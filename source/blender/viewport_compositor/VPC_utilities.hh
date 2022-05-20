/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "BLI_function_ref.hh"
#include "BLI_math_vec_types.hh"

#include "NOD_derived_node_tree.hh"

#include "GPU_shader.h"

#include "VPC_input_descriptor.hh"
#include "VPC_result.hh"

namespace blender::viewport_compositor {

using namespace nodes::derived_node_tree_types;

/* Get the origin socket of the given node input. If the input is not linked, the socket itself is
 * returned. If the input is linked, the socket that is linked to it is returned, which could
 * either be an input or an output. An input socket is returned when the given input is connected
 * to an unlinked input of a group input node. */
DSocket get_input_origin_socket(DInputSocket input);

/* Get the output socket linked to the given node input. If the input is not linked to an output, a
 * null output is returned. */
DOutputSocket get_output_linked_to_input(DInputSocket input);

/* Get the result type that corresponds to the type of the given socket. */
ResultType get_node_socket_result_type(const SocketRef *socket);

/* Returns true if any of the nodes linked to the given output satisfies the given condition, and
 * false otherwise.  */
bool is_output_linked_to_node_conditioned(DOutputSocket output,
                                          FunctionRef<bool(DNode)> condition);

/* Returns the number of inputs linked to the given output that satisfy the given condition.  */
int number_of_inputs_linked_to_output_conditioned(DOutputSocket output,
                                                  FunctionRef<bool(DInputSocket)> condition);

/* A node is a GPU material node if it defines a method to get a GPU material node operation. */
bool is_gpu_material_node(DNode node);

/* Get the input descriptor of the given input socket. */
InputDescriptor input_descriptor_from_input_socket(const InputSocketRef *socket);

/* Dispatch the given compute shader in a 2D compute space such that the number of invocations in
 * both dimensions is as small as possible but at least covers the entirety of global_size assuming
 * the shader has a local group size given by local_size. That means that the number of invocations
 * might be a bit larger than global_size, so shaders has to put that into consideration. A default
 * local size of 16x16 is assumed, which is the optimal local size of image processing when no
 * local memory is used. */
void compute_dispatch_global(GPUShader *shader, int2 global_size, int2 local_size = int2(16));

}  // namespace blender::viewport_compositor