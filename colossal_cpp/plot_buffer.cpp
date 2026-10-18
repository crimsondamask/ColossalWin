#include "plot_buffer.h"

PlotDataBuffer::PlotDataBuffer()

{
    tag_values.reserve(PLOT_BUFFER_INIT_SIZE);
    timestamp_values.reserve(PLOT_BUFFER_INIT_SIZE);
}

double *PlotDataBuffer::tag_data()
{
    return tag_values.data();
}
double *PlotDataBuffer::time_data()
{
    return timestamp_values.data();
}

size_t PlotDataBuffer::tag_data_size()
{
    return tag_values.size();
}

void PlotDataBuffer::push_value(double value, double timestamp)
{
    tag_values.push_back(value);
    timestamp_values.push_back(timestamp);
}
