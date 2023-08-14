const float PI     = 3.1415926;
const float TWO_PI = 6.2831852;

float magnitude(vec2 vec)
{
    return sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

float angleBetween(vec2 v1, vec2 v2)
{
    return atan( v1.x - v2.x, v1.y - v2.y ) + PI;
}

float getTargetAngle() 
{
    return clamp( iTime, 0.0, TWO_PI );
}

// OpenGL uses upper left as origin by default
bool shouldDrawFragment(vec2 fragCoord)
{
    float targetAngle = getTargetAngle();

    float centerX = iResolution.x / 2.0;
    float centerY = iResolution.y / 2.0;
    vec2 center = vec2(centerX, centerY);

    float a = angleBetween(center, fragCoord );

    return a <= targetAngle;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  vec2 uv = fragCoord.xy / iResolution.xy;
  if (shouldDrawFragment(fragCoord)) {
    fragColor = texture(iChannel0, vec2(uv.x, -uv.y));
  } else {
    fragColor = texture(iChannel1, vec2(uv.x, -uv.y));
  }
}