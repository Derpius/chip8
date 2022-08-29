#version 330

#define PI 3.1415926535897932384626433832795

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D screen;
uniform vec2 resolution;

uniform float curvature;
uniform vec3 crtBorderColour;

const float gridlineHalfWidth = 0.01f;

vec2 curveRemapUV(vec2 uv)
{
	uv = uv * 2.f - 1.f;
	vec2 offset = abs(uv.yx) / curvature;
	uv = uv + uv * offset * offset;
	uv = uv * 0.5f + 0.5f;
	return uv;
}

vec4 scanLineIntensity(in vec2 uv)
{
	float count = resolution.y * 1.3f;
	vec2 sl = vec2(sin(uv.y * count), cos(uv.y * count));
	return vec4(sl.x, sl.y, sl.x, 1);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / resolution;
	vec2 remappedUV = curveRemapUV(uv * 1.1f - 0.05f);
	vec4 scrSample = texture(screen, remappedUV);

	if (remappedUV.x < 0.f || remappedUV.y < 0.f || remappedUV.x > 1.f || remappedUV.y > 1.f) {
		// Calculate centred uv
		vec2 uvCentre = uv - 0.5f;

		// Calculate shading
		vec3 albedo = vec3(196.f / 255.f, 192.f / 255.f, 170.f / 255.f);
		if (uvCentre.y > uvCentre.x) {
			if (uvCentre.y > -uvCentre.x)
				albedo *= 0.6f;
			else
				albedo *= 0.8f;
		} else {
			if (uvCentre.y > -uvCentre.x)
				albedo *= 0.95f;
			else
				albedo *= 1.f;
		}

		finalColor = vec4(albedo, 1);
	} else {
		// Reduce contrast
		scrSample = mix(scrSample, vec4(0.5, 0.5, 0.5, 1), 0.2f);

		// Apply scanlines
		scrSample += scrSample * scanLineIntensity(remappedUV) * 0.2f;

		// Output
		finalColor = scrSample;
	}
}
