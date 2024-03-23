unsigned int addCube (RTCScene scene_i)						/* adds a cube to the scene */

unsigned int addGroundPlane (RTCScene scene_i)				/* adds a ground plane to the scene */

extern "C" void device_init (char* cfg)						/* called by the C++ code for initialization */

/* called by the C++ code to render */
extern "C" void renderFrameStandard (int* pixels, width, height, time, ISPCCamera& camera)
	const int numTilesX = (width +TILE_SIZE_X-1)/TILE_SIZE_X;
	const int numTilesY = (height+TILE_SIZE_Y-1)/TILE_SIZE_Y;
	// Threading Building Blocks
	// https://chryswoods.com/parallel_c++/parallel_for.html#:~:text=The%20tbb%3A%3Aparallel_for%20function,calculated%20by%20an%20earlier%20iteration.
  // tbb::parallel_for( range, kernel );
	parallel_for(size_t(0),size_t(numTilesX*numTilesY),[&](const range<size_t>& range) {
		const int threadIndex = (int)TaskScheduler::threadIndex();
		for (size_t i=range.begin(); i<range.end(); i++)
			renderTileTask((int)i,threadIndex,pixels,width,height,time,camera,numTilesX,numTilesY);
	})
			/* task that renders a single screen tile */
			void renderTileTask (taskIndex, threadIndex, pixels, width, height, time, camera, numTilesX, numTilesY)
				const unsigned int tileY = taskIndex / numTilesX;
				const unsigned int tileX = taskIndex - tileY * numTilesX;
				const unsigned int x0 = tileX * TILE_SIZE_X;
				const unsigned int x1 = min(x0+TILE_SIZE_X,width);
				const unsigned int y0 = tileY * TILE_SIZE_Y;
				const unsigned int y1 = min(y0+TILE_SIZE_Y,height);
				for (unsigned int y=y0; y<y1; y++)
					for (unsigned int x=x0; x<x1; x++)
						renderPixelStandard(data,x,y,pixels,width,height,time,camera,g_stats[threadIndex]);

						/* task that renders a single screen tile */
						void renderPixelStandard(data, x, y, pixels, width, height, time, camera, stats)
							RTCIntersectContext context;
							rtcInitIntersectContext(&context);
							/* initialize ray */
							Ray ray(Vec3fa(camera.xfm.p), Vec3fa(normalize(x*camera.xfm.l.vx + y*camera.xfm.l.vy + camera.xfm.l.vz)), 0.0f, inf);
							/* intersect ray with scene */
							rtcIntersect1(data.g_scene,&context,RTCRayHit_(ray));
							RayStats_addRay(stats);
							/* shade pixels */
							Vec3fa color = Vec3fa(0.0f);
							if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
								Vec3fa diffuse  = data.face_colors[ray.primID];
								color           = color + diffuse*0.5f;
								Vec3fa lightDir = normalize(Vec3fa(-1,-1,-1));
								/* initialize shadow ray */
								Ray shadow(ray.org + ray.tfar*ray.dir, neg(lightDir), 0.001f, inf, 0.0f);
								/* trace shadow ray */
								rtcOccluded1(data.g_scene,&context,RTCRay_(shadow));
								RayStats_addShadowRay(stats);
								/* add light contribution */
								if (shadow.tfar >= 0.0f)
									color = color + diffuse*clamp(-dot(lightDir,normalize(ray.Ng)),0.0f,1.0f);

							/* write color to framebuffer */
							unsigned int r = (unsigned int) (255.0f * clamp(color.x,0.0f,1.0f));
							unsigned int g = (unsigned int) (255.0f * clamp(color.y,0.0f,1.0f));
							unsigned int b = (unsigned int) (255.0f * clamp(color.z,0.0f,1.0f));
							pixels[y*width+x] = (b << 16) + (g << 8) + r;

extern "C" void device_render (pixels, width, height, time, camera)			/* called by the C++ code to render */

extern "C" void device_cleanup ()																				/* called by the C++ code for cleanup */
	TutorialData_Destructor(&data);