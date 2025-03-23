#ifndef G_NEURAL_NETWORK
#define G_NEURAL_NETWORK

#include "math-utils.h"

class NeuralNetwork {
public:
  uint8_t node_amount_i = 1, node_amount_h = 1, node_amount_o = 1;
  uint16_t weight_amount_i = 1, weight_amount_o = 1;
  std::vector<float> biases_h, biases_o, weights_i, weights_o, nodes_h, nodes_o;

  NeuralNetwork();

  // Sets amount of nodes in each layer
  void set_node_amount(uint8_t input, uint8_t hidden, uint8_t output);

  void randomize_ratios();

  // Executes calculation cycle with given inputs
  void calculate(const std::vector<float>& input);
};

#endif