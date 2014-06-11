
#include <vector>

int main() {
  std::vector<int> vec;
  std::vector<int> *vec2 = new std::vector<int>();

  #pragma acre ignore
  for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it);
  for (std::vector<int>::iterator it = vec2->begin(); it != vec2->end(); ++it);
  
  for (std::vector<int>::iterator it = vec2->begin(); *it > vec2->back(); ++it);
  
  for (std::vector<int>::iterator it = vec2->begin(); it != vec2->end() && it != vec.end(); ++it);
  
  for (std::vector<int>::iterator it = vec2->begin(), E = vec2->end(); it != E; ++it);

  return 0;
}
