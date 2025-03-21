// Copyright 2023 Nesterov Alexander
#define _USE_MATH_DEFINES

#include <gtest/gtest.h>

#include <random>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "seq/malyshev_a_two_dim_global_optimization_characteristics/include/ops_seq.hpp"

namespace malyshev_a_two_dim_global_optimization_characteristics_seq {
double target(double x, double y) { return x * x + y * y + 10 * sin(x) * cos(y); }

bool constraint1(double x, double y) {
  const int max_iter = 500;
  double zx = x;
  double zy = y;
  for (int i = 0; i < max_iter; i++) {
    double xtemp = zx * zx - zy * zy + x;
    zy = 2 * zx * zy + y;
    zx = xtemp;
    if (zx * zx + zy * zy > 4) return false;
  }
  return true;
}

bool constraint2(double x, double y) {
  const int steps = 90;
  double dx = std::abs(x) / steps;
  double dy = std::abs(y) / steps;
  double integral = 0.0;

  auto f = [](double x, double y) {
    return std::sin(x * x + y * y) * std::exp(-0.1 * (x * x + y * y)) * std::cos(x * y);
  };

  for (int i = 0; i < steps; i++) {
    for (int j = 0; j < steps; j++) {
      double xi = std::min(x, 0.0) + i * dx;
      double yj = std::min(y, 0.0) + j * dy;

      integral += f(xi, yj) * dx * dy;
    }
  }

  return integral <= 10.0;
}

bool constraint3(double x, double y) {
  const int steps = 800;
  double dt = 0.05;
  double u = x;
  double v = y;

  for (int i = 0; i < steps; i++) {
    double k1u = v;
    double k1v = -sin(u);

    double k2u = v + dt * k1v / 2;
    double k2v = -sin(u + dt * k1u / 2);

    double k3u = v + dt * k2v / 2;
    double k3v = -sin(u + dt * k2u / 2);

    double k4u = v + dt * k3v;
    double k4v = -sin(u + dt * k3u);

    u += dt * (k1u + 2 * k2u + 2 * k3u + k4u) / 6;
    v += dt * (k1v + 2 * k2v + 2 * k3v + k4v) / 6;

    if (std::abs(u) > 10 || std::abs(v) > 10) return false;
  }
  return true;
}

bool constraint4(double x, double y) {
  const int terms = 300;
  double sum = 0.0;

  for (int n = 1; n <= terms; n++) {
    sum += std::sin(n * x) * std::cos(n * y) / n;
  }

  return std::abs(sum) <= 2.0;
}
}  // namespace malyshev_a_two_dim_global_optimization_characteristics_seq

TEST(malyshev_a_two_dim_global_optimization_characteristics_seq, test_pipeline_run) {
  std::vector<malyshev_a_two_dim_global_optimization_characteristics_seq::constraint_t> constraints{
      malyshev_a_two_dim_global_optimization_characteristics_seq::constraint1,
      malyshev_a_two_dim_global_optimization_characteristics_seq::constraint2,
      malyshev_a_two_dim_global_optimization_characteristics_seq::constraint3,
      malyshev_a_two_dim_global_optimization_characteristics_seq::constraint4,
  };

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();

  malyshev_a_two_dim_global_optimization_characteristics_seq::Point res;
  std::pair<double, double> bounds_[2]{{-0.5, 0.5}, {-0.5, 0.5}};
  double eps = 1e-4;

  taskData->inputs.push_back(reinterpret_cast<uint8_t *>(&bounds_));
  taskData->inputs.push_back(reinterpret_cast<uint8_t *>(&eps));
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t *>(&res));
  taskData->inputs_count.push_back(2);
  taskData->inputs_count.push_back(1);
  taskData->outputs_count.push_back(1);

  auto task = std::make_shared<malyshev_a_two_dim_global_optimization_characteristics_seq::TestTaskSequential>(
      taskData, malyshev_a_two_dim_global_optimization_characteristics_seq::target, constraints);

  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(task);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
}

TEST(malyshev_a_two_dim_global_optimization_characteristics_seq, test_task_run) {
  std::vector<malyshev_a_two_dim_global_optimization_characteristics_seq::constraint_t> constraints{
      malyshev_a_two_dim_global_optimization_characteristics_seq::constraint1,
      malyshev_a_two_dim_global_optimization_characteristics_seq::constraint2,
      malyshev_a_two_dim_global_optimization_characteristics_seq::constraint3,
      malyshev_a_two_dim_global_optimization_characteristics_seq::constraint4,
  };

  std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();

  malyshev_a_two_dim_global_optimization_characteristics_seq::Point res;
  std::pair<double, double> bounds_[2]{{-0.5, 0.5}, {-0.5, 0.5}};
  double eps = 1e-4;

  taskData->inputs.push_back(reinterpret_cast<uint8_t *>(&bounds_));
  taskData->inputs.push_back(reinterpret_cast<uint8_t *>(&eps));
  taskData->outputs.emplace_back(reinterpret_cast<uint8_t *>(&res));
  taskData->inputs_count.push_back(2);
  taskData->inputs_count.push_back(1);
  taskData->outputs_count.push_back(1);

  auto task = std::make_shared<malyshev_a_two_dim_global_optimization_characteristics_seq::TestTaskSequential>(
      taskData, malyshev_a_two_dim_global_optimization_characteristics_seq::target, constraints);

  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perfAttr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  auto perfResults = std::make_shared<ppc::core::PerfResults>();

  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(task);
  perfAnalyzer->task_run(perfAttr, perfResults);
  ppc::core::Perf::print_perf_statistic(perfResults);
}