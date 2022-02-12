#shader vertex
#version 330 core
layout(location = 0) in vec3 position;    // 顶点坐标为0
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); //注意乘的顺序要相反 即 pvm
};
#shader fragment
#version 330 core
out vec4 color;
void main()
{
	color = vec4(1.0f); //设置四维向量的所有元素为 1.0f
};