#pragma once
#include <gtest/gtest.h>

#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "core/task/include/task.hpp"

namespace kovalchuk_a_horizontal_tape_scheme_seq {

const int MINIMALGEN = -999;
const int MAXIMUMGEN = 999;

class TestSequentialTask : public ppc::core::Task {
 public:
  explicit TestSequentialTask(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(std::move(taskData_)) {}
  bool pre_processing() override;
  bool validation() override;
  bool run() override;
  bool post_processing() override;

 private:
  std::vector<std::vector<int>> matrix_;
  std::vector<int> vector_;
  std::vector<int> result_;
};

}  // namespace kovalchuk_a_horizontal_tape_scheme_seq