#shader vertex
#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 g_texCoords;

out vec2 texCoords;

void main() {
    gl_Position = vec4(inPos.x, inPos.y, 0, 1);
    texCoords = g_texCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float gamma;
uniform float exposure;

// post processing uniforms
uniform float chromaticAmount;
uniform float vignetteAmount;

vec4 chromaticAberration(float amount)
{
    float r = texture(screenTexture, texCoords - vec2(amount, 0)).r;
    float g = texture(screenTexture, texCoords).g;
    float b = texture(screenTexture, texCoords + vec2(amount, 0)).b;

    return vec4(r, g, b, texture(screenTexture, texCoords).a);
}

vec4 vignette(float amount)
{
    vec2 tex = texCoords - vec2(0.5, 0.5);
    float dist = length(tex);
    float vignette = smoothstep(0.8, 0.0, dist * (amount + 0.5));
    return vec4(vignette, vignette, vignette, 1.0);
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 filmGrain(float amount) {
    float grain = (rand(texCoords) - 0.5) * amount;

    return vec4(grain, grain, grain, 1.0);
}

vec4 radialBlur(vec4 tex, vec2 cursorPos, float blurWidth) {
    vec4 color = tex;
	vec2 res;
	res = texCoords;
	vec2 pp = cursorPos;
	vec2 center = pp;
	float blurStart = 1.0;

    const int nsamples = 10;

	vec2 uv = texCoords.xy;

	uv -= center;
	float precompute = blurWidth * (1.0 / float(nsamples - 1));

	for(int i = 0; i < nsamples; i++)
	{
		float scale = blurStart + (float(i)* precompute);
		float r = texture2D(screenTexture, (uv * scale + center) - vec2(chromaticAmount, 0.0)).rgb.x;
		float g = texture2D(screenTexture, (uv * scale + center)).rgb.y;
		float b = texture2D(screenTexture, (uv * scale + center) + vec2(chromaticAmount, 0.0)).rgb.z;
		float a = texture2D(screenTexture, (uv * scale + center)).a;

		color += vec4(r, g, b, a);
	}

	color /= float(nsamples);
    return color;
}

vec4 bloom()
{
    // vec3 color = radialBlur(chromaticAberration(chromaticAmount), vec2(0.5, 0.5), 0.05).rgb;
    vec3 color = chromaticAberration(chromaticAmount).rgb;
    vec3 bloomColor = texture(bloomTexture, texCoords).rgb;
    return vec4(color + bloomColor, 1.0) * vignette(vignetteAmount);
}

void main() {
    vec2 resolution = vec2(1920, 1080);
    vec2 srcTexel = 1.0 / resolution;
    float x = srcTexel.x;
    float y = srcTexel.y;

    vec4 fragment = texture(screenTexture, texCoords.st);
    vec4 color = bloom();

    vec3 toneMapped = vec3(1.0) - exp(-color.rgb * exposure);

    vec3 res = vec3(0.0);
    float Z = 0.0;

    FragColor.rgb = pow(toneMapped, vec3(1.0/2.2));
}
