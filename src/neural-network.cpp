#include "neural-network.h"

NeuralNetwork::NeuralNetwork() {

}

void NeuralNetwork::set_node_amount(uint8_t input, uint8_t hidden, uint8_t output) {
  node_amount_i = input;
  node_amount_h = hidden;
  node_amount_o = output;

  biases_h.resize(hidden);
  biases_o.resize(output);

  nodes_h.resize(hidden);
  nodes_o.resize(output);

  weight_amount_i = input * hidden;
  weight_amount_o = hidden * output;

  weights_i.resize(weight_amount_i);
  weights_o.resize(weight_amount_o);
}

void randomize_n_ratios(std::vector<float>& vec) {
  for (float& v : vec)
    v = randfr();
}

void NeuralNetwork::randomize_ratios() {
  randomize_n_ratios(biases_h);
  randomize_n_ratios(biases_o);
  randomize_n_ratios(weights_i);
  randomize_n_ratios(weights_o);
}

// (-1; 1)
void activation_function(float& x) {
  x = tanh(x);
}

void NeuralNetwork::calculate(const std::vector<float>& input) {
  nodes_h = biases_h;
  nodes_o = biases_o;

  int m = 0;
  for (int i = 0; i < node_amount_i; ++i)
    for (int n = 0; n < node_amount_h; ++n)
      nodes_h[n] += input[i] * weights_i[m++];
  for (float& node : nodes_h)
    activation_function(node);
  m = 0;
  for (int i = 0; i < node_amount_h; ++i)
    for (int n = 0; n < node_amount_o; ++n)
      nodes_o[n] += nodes_h[i] * weights_o[m++];
  for (float& node : nodes_o)
    activation_function(node);
}