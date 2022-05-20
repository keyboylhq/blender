/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <memory>

#include "BLI_map.hh"
#include "BLI_string_ref.hh"
#include "BLI_vector.hh"

#include "DNA_node_types.h"

#include "NOD_derived_node_tree.hh"
#include "NOD_node_declaration.hh"

#include "VPC_context.hh"
#include "VPC_input_descriptor.hh"
#include "VPC_node_operation.hh"
#include "VPC_operation.hh"
#include "VPC_result.hh"
#include "VPC_scheduler.hh"
#include "VPC_utilities.hh"

namespace blender::viewport_compositor {

using namespace nodes::derived_node_tree_types;

NodeOperation::NodeOperation(Context &context, DNode node) : Operation(context), node_(node)
{
  /* Populate the output results. */
  for (const OutputSocketRef *output : node->outputs()) {
    const ResultType result_type = get_node_socket_result_type(output);
    const Result result = Result(result_type, texture_pool());
    populate_result(output->identifier(), result);
  }

  /* Populate the input descriptors. */
  for (const InputSocketRef *input : node->inputs()) {
    const InputDescriptor input_descriptor = input_descriptor_from_input_socket(input);
    declare_input_descriptor(input->identifier(), input_descriptor);
  }
}

void NodeOperation::compute_results_reference_counts(const Schedule &schedule)
{
  for (const OutputSocketRef *output_ref : node()->outputs()) {
    const DOutputSocket output{node().context(), output_ref};

    const int reference_count = number_of_inputs_linked_to_output_conditioned(
        output, [&](DInputSocket input) { return schedule.contains(input.node()); });

    get_result(output->identifier()).set_initial_reference_count(reference_count);
  }
}

const DNode &NodeOperation::node() const
{
  return node_;
}

const bNode &NodeOperation::bnode() const
{
  return *node_->bnode();
}

bool NodeOperation::should_compute_output(StringRef identifier)
{
  return get_result(identifier).should_compute();
}

}  // namespace blender::viewport_compositor