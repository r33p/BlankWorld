#include <iostream>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


int main(){
	glm::mat4 trans;
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 result = trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	printf("%f, %f, %f\n", result.x, result.y, result.z);
}
