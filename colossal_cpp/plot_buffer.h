#include "data_buffer.h"
#include "link.h"
#include <vector>

#define PLOT_BUFFER_INIT_SIZE 900

class PlotDataBuffer
{
  public:
    PlotDataBuffer();
    ~PlotDataBuffer() = default;

    double *tag_data();
    double *time_data();
    void push_value(double value, double timestamp);
    size_t tag_data_size();
    void clear();

  private:
    std::vector<double> tag_values;
    std::vector<double> timestamp_values;
};
