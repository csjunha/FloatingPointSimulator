#include "fp.h"

FP::FP(const FP &other)
{
    this->size = other.size;
    this->exp_size = other.exp_size;
    this->man_size = other.man_size;

    this->sign = other.sign;
    this->exp = other.exp;

    this->man = other.man;
    this->int_offset = other.int_offset;
    this->frac_offset = other.frac_offset;

    this->bias = other.bias;
    this->exp_max = other.exp_max;
}

FP::FP(uint64_t data, offset_t exp_size, offset_t man_size)
{
    this->size = 1 + exp_size + man_size;
    this->exp_size = exp_size;
    this->man_size = man_size;

    uint64_t exp_mask = get_bitmask_64(this->size - 2, man_size);
    uint64_t man_mask = get_bitmask_64(man_size - 1, 0);

    exp_t exp_bits = (exp_t)((data & exp_mask) >> man_size);
    uman_t man_bits = data & man_mask;

    bool is_subnormal = (exp_bits == 0);
    bool is_inf = (exp_bits == ((exp_t)1 << exp_size) - 1);

    // unsigned mantissa
    uman_t uman = is_subnormal ? man_bits : (((uman_t)1 << man_size) | man_bits);

    this->sign = (data & ((uint64_t)1 << (this->size - 1))) ? true : false;
    this->exp = is_subnormal ? (uman == 0 ? 0 : 1) : exp_bits;
    this->man = is_inf ? 0 : sign ? -(sman_t)uman
                                  : (sman_t)uman;

    this->bias = ((exp_t)1 << (exp_size - 1)) - 1;
    this->exp_max = ((exp_t)1 << exp_size) - 1;

    this->int_offset = man_size + 1;
    this->frac_offset = man_size - 1;
}

FP::FP(
    offset_t exp_size,
    offset_t man_size,
    sman_t sman,
    exp_t exp,
    offset_t int_offset,
    offset_t frac_offset)
{
    this->size = 1 + exp_size + man_size;
    this->exp_size = exp_size;
    this->man_size = man_size;

    this->sign = (sman & ((uint64_t)1 << int_offset)) ? true : false;
    this->man = sman;
    this->exp = exp;

    this->bias = ((exp_t)1 << (exp_size - 1)) - 1;
    this->exp_max = ((exp_t)1 << exp_size) - 1;

    this->int_offset = int_offset;
    this->frac_offset = frac_offset;
}

uman_t FP::get_uman()
{
    return this->sign ? -this->man : this->man;
}

bool FP::is_compatible_with(const FP &other) const
{
    return (this->size == other.size &&
            this->exp_size == other.exp_size &&
            this->man_size == other.man_size &&
            this->int_offset == other.int_offset &&
            this->frac_offset == other.frac_offset);
}

void FP::pad(offset_t padamt)
{
    this->int_offset += padamt;
    this->frac_offset += padamt;
    this->man <<= padamt;
}

void FP::normalize()
{
    if (this->is_normalized())
    {
        return;
    }

    if (this->exp >= this->exp_max)
    {
        this->man = 0;
        this->exp = this->exp_max;
        this->normalize_offsets();
        return;
    }
    if (this->man == 0)
    {
        this->exp = 0;
        this->normalize_offsets();
        return;
    }

    // Check if signed mantissa is out of range
    if (this->is_sman_out_of_range())
    {
        bool inf = this->exp + 1 >= this->exp_max;
        this->exp = inf ? this->exp_max : this->exp + 1;
        this->man = inf ? 0 : this->man >> 1;
        if (inf)
        {
            this->normalize_offsets();
            return;
        }
    }

    exp_t upnorm_shamt = this->get_upnorm_shamt();
    exp_t downnorm_shamt = this->get_downnorm_shamt();

    if (upnorm_shamt)
    {
        bool inf = (this->exp + upnorm_shamt) >= this->exp_max;
        this->exp = inf ? this->exp_max : this->exp + upnorm_shamt;
        this->man = inf ? 0 : this->man >> upnorm_shamt;
        if (inf)
        {
            this->normalize_offsets();
            return;
        }
    }
    else if (downnorm_shamt)
    {
        bool over = this->exp < downnorm_shamt;
        downnorm_shamt = over ? this->exp : downnorm_shamt;
        this->exp = over ? 0 : this->exp - downnorm_shamt;
        this->man <<= downnorm_shamt;
    }

    this->int_offset = this->frac_offset + 2;
}

void FP::round()
{
    assert(this->int_offset >= this->frac_offset + 2);
    assert(this->frac_offset >= this->man_size - 1);

    if (this->is_rounded())
    {
        return;
    }

    offset_t padamt = this->get_padbits_width();
    assert(padamt > 0);

    offset_t uint_bits = this->int_offset - this->frac_offset - 1;
    uman_t round = this->should_round_up() ? 1 : 0;

    uman_t uman = this->get_uman();
    uman_t actual_uman = (uman & get_bitmask_64(this->int_offset - 1, padamt)) >> padamt;
    uman_t actual_uman_round = actual_uman + round;

    // Should we re-upnormalize?
    bool carry = (actual_uman_round & (1 << (uint_bits + this->man_size))) ? true : false;
    uman_t uman_round = carry ? actual_uman_round >> 1 : actual_uman_round;
    sman_t sman_round = this->sign ? -(sman_t)uman_round : (sman_t)uman_round;

    if (carry)
    {
        // Re-upnormalize
        bool inf = this->exp + 1 >= this->exp_max;
        this->exp = inf ? this->exp_max : this->exp + 1;
        if (inf)
        {
            this->man = 0;
            this->normalize_offsets();
            return;
        }
    }

    this->man = sman_round;
    this->int_offset -= padamt;
    this->frac_offset -= padamt;
}

uint64_t FP::encode()
{
    assert(this->is_normalized_and_rounded());

    if (this->exp >= this->exp_max)
    {
        return INF(this->size, this->sign, this->man_size, this->exp_size);
    }
    if (this->man == 0)
    {
        return 0;
    }

    if (this->is_sman_out_of_range())
    {
        this->exp += 1;
        this->man >>= 1;
        if (this->exp >= this->exp_max)
        {
            this->man = 0;
            return INF(this->size, this->sign, this->man_size, this->exp_size);
        }
    }

    uman_t uman = this->get_uman();
    offset_t uman_high = this->int_offset - 1;

    if (this->exp == 0)
    {
        // In this case, we cannot represent it as IEEE754 floating point number
        // We should add 1 to the exponent and shift the mantissa to the right
        uman_t uman_lsb = uman & (uint64_t)1;
        uman_t uman_remainder = (uman & get_bitmask_64(uman_high, 1)) >> 1;
        uman = uman_remainder + uman_lsb;
    }

    bool uman_msb = uman & ((uman_t)1 << uman_high) ? true : false;
    assert(!uman_msb ? this->exp == 0 : true); // If uman_msb is not set, exp should be 0 to conform subnormal number

    uint64_t mantissa = uman & get_bitmask_64(uman_high - 1, 0);
    uint64_t exp = this->exp == 0 ? uman_msb ? 1 : 0 : this->exp;

    return (((uint64_t)(this->sign ? 1 : 0) << (this->size - 1)) |
            (exp << this->man_size) |
            mantissa) &
           get_bitmask_64(this->size - 1, 0);
}

void FP::clear()
{
    this->sign = false;
    this->exp = 0;
    this->man = 0;
    this->int_offset = this->man_size + 1;
    this->frac_offset = this->man_size - 1;
}

void FP::dump(const char *tag)
{
    printf("============= Start of %s =============\n\n", tag);
    printf("Sign: %d\n", this->sign);
    printf("Exp: %d\n", this->exp);
    printf("Bias: %d\n", this->bias);

    this->dump_mantissa();

    printf("Normalized: %s\n", this->is_normalized() ? "true" : "false");
    printf("Rounded: %s\n", this->is_rounded() ? "true" : "false");

    printf("Pad bits: %d\n", this->get_padbits_width());

    printf("Int offset: %d\n", this->int_offset);
    printf("Frac offset: %d\n", this->frac_offset);

    if (this->is_normalized_and_rounded())
    {
        dump_bits("Encoded", this->encode(), this->size);
        printf("Float (approx): %.16f\n", this->to_float());
    }
    printf("\n");
    printf("============= End of %s =============\n\n", tag);
}

float FP::to_float()
{
    int32_t exponent = (int32_t)this->exp - (int32_t)this->bias;

    float result = 0.0f;
    float coeff = powf32(2.0f, (float)(this->int_offset - this->frac_offset - 1));
    for (int i = this->int_offset; i >= 0; i--)
    {
        uint64_t mask = (uint64_t)1 << i;
        bool on = this->man & mask ? true : false;
        result += on ? i == this->int_offset ? -coeff : coeff : 0.0f;
        coeff /= 2.0f;
    }

    return result * powf32(2.0f, (float)exponent);
}

FP &FP::operator=(const FP &other)
{
    this->size = other.size;
    this->exp_size = other.exp_size;
    this->man_size = other.man_size;

    this->sign = other.sign;
    this->exp = other.exp;

    this->man = other.man;
    this->int_offset = other.int_offset;
    this->frac_offset = other.frac_offset;

    this->bias = other.bias;
    this->exp_max = other.exp_max;

    return *this;
}

FP FP::operator+(const FP &other) const
{
    GlobalConfig &config = GlobalConfig::get_instance();

    FP this_copy(*this);
    FP other_copy(other);

    this_copy.normalize();
    this_copy.round();
    other_copy.normalize();
    other_copy.round();

    assert(this_copy.is_compatible_with(other_copy));

    if (this_copy.exp >= this_copy.exp_max || other_copy.exp >= other_copy.exp_max)
    {
        // If any of the operands is infinity, return infinity
        return FP(
            INF(this_copy.size, 0, this_copy.man_size, this_copy.exp_size),
            this_copy.exp_size,
            this_copy.man_size);
    }

    this_copy.pad(config.get_add_pad_width());
    other_copy.pad(config.get_add_pad_width());

    offset_t result_int_offset = this_copy.int_offset + 1;
    offset_t result_frac_offset = this_copy.frac_offset;

    bool compare = this_copy.exp < other_copy.exp;
    FP *smaller = compare ? &this_copy : &other_copy;
    FP *larger = compare ? &other_copy : &this_copy;

    exp_t exp_max = larger->exp;
    exp_t shamt = larger->exp - smaller->exp;

    sman_t smaller_sman = smaller->man >> shamt;
    sman_t sman_sum = larger->man + smaller_sman;

    FP result(
        this_copy.exp_size,
        this_copy.man_size,
        sman_sum,
        exp_max,
        result_int_offset,
        result_frac_offset);

    result.normalize();
    result.round();

    return result;
}

bool FP::should_round_up()
{
    if (this->frac_offset <= this->man_size - 1)
    {
        return false;
    }

    offset_t padamt = this->get_padbits_width();
    uman_t uman = this->get_uman();

    uman_t msb_padbits = uman & ((uman_t)1 << (padamt - 1));
    if (padamt == 1)
    {
        return msb_padbits ? true : false;
    }

    uman_t last_bit_before_pad = uman & ((uman_t)1 << padamt);
    uman_t remainder = uman & (((uman_t)1 << (padamt - 1)) - 1);

    return last_bit_before_pad ? (msb_padbits ? true : false)
                               : (msb_padbits ? true : false) && (remainder != 0);
}

offset_t FP::get_padbits_width()
{
    return this->frac_offset - this->man_size + 1;
}

offset_t FP::get_upnorm_shamt()
{
    offset_t shamt = 0;
    uman_t uman = this->get_uman();

    offset_t high = this->int_offset - 1;
    offset_t low = this->frac_offset + 1;

    for (int i = (int)high; i >= (int)low; --i)
    {
        uman_t mask = (uman_t)1 << i;
        if (uman & mask)
        {
            shamt = i - low;
            break;
        }
    }

    return shamt;
}

offset_t FP::get_downnorm_shamt()
{
    offset_t lz = 0;
    uman_t uman = this->get_uman();

    offset_t high = this->frac_offset + 1;
    offset_t low = 0;

    for (int i = (int)high; i >= (int)low; --i)
    {
        uman_t mask = (uman_t)1 << i;
        if (uman & mask)
        {
            lz = high - i;
            break;
        }
    }

    return lz;
}

//
// This function is to detect if the signed mantissa is out of range.
// 1000...0 is the smallest negative number, and 0111...1 is the largest, but
// 1000...0 is not representable in unsigned mantissa which is 1 bit shorter.
//
bool FP::is_sman_out_of_range()
{
    uman_t msb_mask = (uman_t)1 << (this->int_offset);
    uman_t remainder_mask = msb_mask - 1;

    if ((this->man & msb_mask) && !(this->man & remainder_mask))
    {
        return true;
    }
    return false;
}

inline bool FP::is_normalized()
{
    return this->int_offset - this->frac_offset == 2;
}

inline bool FP::is_rounded()
{
    return this->get_padbits_width() == 0;
}

inline bool FP::is_normalized_and_rounded()
{
    return this->is_normalized() && this->is_rounded();
}

inline void FP::normalize_offsets()
{
    this->int_offset = this->frac_offset + 2;
}

void FP::dump_mantissa()
{
    offset_t padbits_width = this->get_padbits_width();

    printf("Mantissa (signed): ");
    offset_t sman_high = this->int_offset;
    for (int i = sman_high; i >= 0; --i)
    {
        bool on = (this->man & (uman_t)1 << i) ? true : false;
        printf("%d", on ? 1 : 0);
        if (i == this->frac_offset + 1)
        {
            printf(".");
        }
        if (padbits_width > 0 && i == padbits_width)
        {
            printf("|");
        }
    }
    printf("\n");

    printf("Mantissa (unsigned): ");
    offset_t uman_high = this->int_offset - 1;
    uman_t uman = this->get_uman();
    for (int i = uman_high; i >= 0; --i)
    {
        bool on = (uman & (uman_t)1 << i) ? true : false;
        printf("%d", on ? 1 : 0);
        if (i == this->frac_offset + 1)
        {
            printf(".");
        }
        if (padbits_width > 0 && i == padbits_width)
        {
            printf("|");
        }
    }
    printf("\n");
}
