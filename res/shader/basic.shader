#shader vertex
#version 330 core
layout(location = 0) in vec3 position;    // 顶点坐标为0
layout(location = 1) in vec3 normal;      // 每个面的法向量
layout(location = 2) in vec2 texCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
void main()
{
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords;
	FragPos = vec3(model * vec4(position, 1.0f));//得到每个顶点属性世界坐标的位置 然后给片段着色器计算漫反射
	// mvp
	gl_Position = projection * view * model * vec4(position, 1.0f); //注意乘的顺序要相反 即 pvm
};
#shader fragment
#version 330 core
struct Material
{
	sampler2D diffuse;//漫反射贴图
	sampler2D specular;//镜面贴图
	float shininess;
};
//平行光
struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
//点光
struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform Material material;//材质
uniform vec3 lightPos;
uniform vec3 viewPos;//观察坐标
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;//光照贴图
out vec4 color;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{
	// 一些属性
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	// 第一步，计算平行光照
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	// 第二步，计算顶点光照
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	// 第三部，计算 Spot light
	//result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	color = vec4(result, 1.0);
};
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// 计算漫反射强度
	float diff = max(dot(normal, lightDir), 0.0);
	// 计算镜面反射强度
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// 合并各个光照分量
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// 计算漫反射强度
	float diff = max(dot(normal, lightDir), 0.0);
	// 计算镜面反射
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// 计算衰减
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance +
		light.quadratic * (distance * distance));
	// 将各个分量合并
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}