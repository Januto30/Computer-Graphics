varying vec3 v_world_normal;
varying vec2 v_uv;
uniform sampler u_face_texture;

void main()
{
	vec3 color = vec3(texture2D(u_face_texture, v_uv)).rgb
	gl_FragColor = vec4( color, 1.0 );
}
