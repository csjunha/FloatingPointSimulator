#include "./accumulator.h"

Accumulator::Accumulator(offset_t exp_size, offset_t man_size)
{
    FP *fp = new FP(0, exp_size, man_size);
    fp->pad(GlobalConfig::get_instance().get_add_pad_width());
    this->acc = fp;
}

Accumulator::~Accumulator()
{
    delete this->acc;
}

void Accumulator::clear()
{
    this->acc->clear();
}

void Accumulator::accumulate(const FP &input)
{
    FP copy(input);
    copy.normalize();
    copy.round();
    copy.pad(GlobalConfig::get_instance().get_add_pad_width());

    assert(copy.is_compatible_with(*this->acc));

    if (copy.exp >= copy.exp_max || this->acc->exp >= this->acc->exp_max)
    {
        // If any of the operands is infinity, make infinity
        this->acc->sign = false;
        this->acc->exp = this->acc->exp_max;
        this->acc->man = 0;
        this->acc->int_offset = this->acc->frac_offset + 2;

        return;
    }

    offset_t result_int_offset = copy.int_offset + 1;
    offset_t result_frac_offset = copy.frac_offset;

    bool compare = this->acc->exp < copy.exp;
    FP *smaller = compare ? this->acc : &copy;
    FP *larger = compare ? &copy : this->acc;

    exp_t exp_max = larger->exp;
    exp_t shamt = larger->exp - smaller->exp;

    sman_t smaller_sman = smaller->man >> shamt;
    sman_t sman_sum = larger->man + smaller_sman;

    this->acc->sign = (sman_sum & ((uint64_t)1 << result_int_offset)) ? true : false;
    this->acc->man = sman_sum;
    this->acc->exp = exp_max;

    this->acc->int_offset = result_int_offset;
    this->acc->frac_offset = result_frac_offset;

    this->acc->normalize();
}

FP Accumulator::get_result()
{
    FP copy(*this->acc);

    copy.normalize();
    copy.round();

    return copy;
}
