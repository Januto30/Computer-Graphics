varying vec2 v_uv;

uniform float u_aspectRatio; //anchura/altura
uniform sampler2D u_texture; 
float limit;
uniform float u_value, b_value, c_value, d_value, e_value, f_value, lletra_value, aug_value;
//uniform sampled2D u_texture;

void main()
{
	if (0 < lletra_value && lletra_value <1.4)
    {
		vec4 color1; 

        if (u_value > 0.5)
        {
			//Creem un color difuminat que varia de blau a vermell en funció de v_uv.x
			vec3 difuminat = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), v_uv.x);

			vec4 color1 = vec4(difuminat, 1.0);
			gl_FragColor = color1;	
        } 

		else if (b_value > 0.5) 
		{
			//Creem un color en blanc i negre basat en la distància a la posició central
			vec3 BlackWhite = vec3(distance(vec2(0.5 * u_aspectRatio, 0.5), v_uv * vec2(u_aspectRatio, 1.0)));
        
			vec4 color1 = vec4(BlackWhite, 1.0);
			gl_FragColor = color1;	
		}

		else if (c_value > 0.5)
		{
			//Creem un efecte de línies vermelles i blaves en funció de v_uv.x i v_uv.y
			float redLine = smoothstep(0.1, 0.2, mod(v_uv.x * 5.0 * u_aspectRatio, 1.0)) - smoothstep(0.8, 0.9, mod(v_uv.x * 5.0 * u_aspectRatio, 1.0));
			float blueLine = smoothstep(0.1, 0.2, mod(v_uv.y * 5.0, 1.0)) - smoothstep(0.8, 0.9, mod(v_uv.y * 5.0, 1.0));

			color1.r = 1.0 - redLine;
			color1.g = 0.0;
			color1.b = 1.0 - blueLine;

			vec4 color1 = vec4(color1.rgb, 1.0);
			gl_FragColor = color1;	
		}

		else if (d_value > 0.5)
		{
			//Definim la mida dels quadrats a la graella.
			float a1 = 1.0 / 16.0;

			//Escal·lem les coordenades de textura segons l'aspect ratio.
			vec2 scaled_uv = v_uv.xy * vec2(u_aspectRatio, 1.0);

			//Trobem la posició del quadrat a la graella mitjançant arrodoniment cap avall
			vec2 grid_uv = floor(scaled_uv / a1) * a1;

			//Ajustem les coordenades per mantenir l'aspect ratio correcte en el resultat final
			gl_FragColor = vec4(grid_uv.xy / vec2(u_aspectRatio, 1.0), 0.0, 1.0);
		}

		else if (e_value > 0.5)
		{
			float scale = 8.0;
			
			//Creem un patró de quadricula en blanc i negre en funció de la posició a la textura
			vec2 pos = mod(v_uv * scale * vec2(u_aspectRatio, 1.0), u_aspectRatio);
			float ch = mod(floor(pos.x) + floor(pos.y), u_aspectRatio);

			vec4 color1 = vec4(mix(vec3(1.0), vec3(0.0), ch), 1.0);
			gl_FragColor = color1;	

		}

		else if (f_value > 0.5)
		{
			float x = (v_uv.x * u_aspectRatio * 3.1416);
			float y = v_uv.y - 0.5;
			float wave = 0.4 * sin(x);


			float val = step(wave,y);
			//Per a cada valor inferior a la sinusoide designem un degradat entre verd i negre.
			vec4 color1 = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), v_uv.y);
			//Per a cada valor superior a la sinusoide designem un degradat entre verd i negre.
			vec4 color2 = mix(vec4(0.0, 1.0, 0.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), v_uv.y);

			gl_FragColor = mix(color1, color2, val);	

		}
		
	}else if (1.6 < lletra_value && lletra_value< 2.4) {
		vec4 color = texture2D(u_texture, v_uv); 
		
		if (u_value > limit){
			//Calculem el nivell de gris utilitzant els valors averiguats amb diferentes probes.
			float gray = dot(color.rgb, vec3(0.3, 0.6, 0.1));
			
			//Converteix el color a escala de gris
			color = vec4(vec3(gray), 1.0);

		} else if (b_value > limit){
			//Invertim els components RGB del color
			color.rgb = vec3(1.0) - color.rgb; 
		
		} else if (c_value > limit){
			//Calculem un nivell de gris utilitzant valors escollits a ull.
			float grayscale = dot(color.rgb, vec3(0.2, 0.6, 0.5));

			//Calculem la quantitat de groc present al color
			color.rgb = vec3(grayscale);
			float yellow = dot(color.rgb, vec3(0.5, 0.5, 0.0));

			//Creem un color groc
			vec3 yellowColor = mix(vec3(1.0, 1.0, 0.0), color.rgb, yellow);
			
			//Mescla el color original amb el color groc
			color.rgb = mix(color.rgb, yellowColor, 0.4);
		
		} else if (d_value > limit){
			// Calcula el nivell de gris amb pesos específics a ull
			float gray = dot(color.rgb, vec3(0.3, 0.5, 0.1));

			//Convertim el color a blanc o negre basant-se en un llindar
			color.rgb = vec3(step(0.5, gray));
		
		} else if (e_value > limit){
			//Calculem la distància des de v_uv fins al punt central (0.5, 0.5)
			float dist = distance(v_uv, vec2(0.5, 0.5));
			color.rgb *= 1.0 - dist; 

		} else if (f_value > limit) {
   
			//Assignem el color a zero (negre)
			color = vec4(0.0);
			
			//Número de mostres per a la mitjana
			float numSamples = 50.0;
			 
			//Mida de la finestra de mostreig al voltant del punt central
			float range = 0.03;

			for (float i = 0.0; i < numSamples; i++) {
				float offset = i - (numSamples - 1.0) / 2.0;
				// Calcula l'offset respecte al punt central
				offset *= range / numSamples;

				// Altera el valor en l'eix x i suma els colors obtinguts
				color += texture2D(u_texture, v_uv + vec2(offset, 0.0));
			}

			// Calcula la mitjana dels colors obtinguts
			color /= numSamples;
		}

		// Assigna el color final a la sortida del fragment shader
		gl_FragColor = color;


	} else if (2.6 < lletra_value && lletra_value< 3.4 ){
		if (u_value > limit){
			float angle = 16;
			float s = sin(angle);
			float c = cos(angle);
			vec2 uv = (v_uv * 2.0 - 1.0) * vec2(u_aspectRatio, 1.0);

			//Aplicar rotació
			uv = mat2(c, -s, s, c) * uv;

			//Descentrar les cordenades de textura i ajustar en funció el aspect_ratio
			uv = uv / vec2(u_aspectRatio, 1.0) * 0.5 + 0.5;

			//Comprobar si les cordenades de textura estan fora la imatge
			float outsideImage = step(uv.x, 0.0) * step(1.0 - uv.x, 0.0) * step(uv.y, 0.0) * step(1.0 - uv.y, 0.0);

			//Assignar el color negre a las coordenades fora de la imatge
			gl_FragColor = mix(vec4(0.0, 0.0, 0.0, 1.0), texture2D(u_texture, uv), 1.0 - outsideImage);


		} else if (b_value > limit){
		    float blockSize = 20.0; //Tamany dels "blocs" de pixelació.

			vec2 position = v_uv * (720+aug_value); //La posició del pixel en pantalla.

			vec2 blockPosition = floor(position / blockSize); //La posició del bloc en pantalla.

			vec2 blockUV = blockPosition / ((720+aug_value) / blockSize); // Les coords de textura del bloc

			vec4 color = texture2D(u_texture, blockUV); //Color del bloc

			gl_FragColor = color;
		}

    }	


		//fILTRE LES ESQUINES I FERLES FOSQUES
		//Imatge borosa: Agafo la suma dels veins i faig la mitja rollo els veiins dels colors del costat.

		//Transformació de imatges:

		//Vec3 color = texture2D(u_texture, v_uv).rgb //xyz lo mismo
    
}


