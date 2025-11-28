#include "pch.h"
#include "RSMergedShaders.h"


/***************************** ComputeCMUDensity ******************************/


const char* ComputeCMUDensity_cs = R"(


#version 460

layout (local_size_x = 256) in;

#define PI_FLOAT 3.1415927410125732421875f
layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[]; // position xyz, density w
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, pressure w
};

uniform float particle_radius;
uniform float smoothing_length = 1.0f;
uniform float particle_mass = 1.0f;
uniform float u_density_0 = 1000.0f;
uniform float u_gas_constant = 2000.0f; // Gas constant for equation of state

// SPH kernel
float Poly6Kernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h * h - r * r);
        return 315.0 / (64.0 * 3.14159265359 * pow(h, 9.0)) * hr * hr * hr;
    } else {
        return 0.0;
    }
}

//// Gradient of the spiky kernel
//float SpikyKernel(float r, float h) {
//    if (r >= 0.0 && r <= h) {
//        float hr = (h - r);
//        return 15.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr * hr;
//    } else {
//        return 0.0;
//    }
//}

// https://arxiv.org/pdf/1706.10263 page 7/26
float KernelFunction(float r, float h)
{
    float s = r / h;
    float section_data = 0.0;

    if ( s >= 0.0 && s < 1.0) 
    {
		section_data =  s * s * s - 6 * s + 6;
	} 
    else if (s >= 1.0 && s < 2.0) {
		section_data =  pow((2-s), 3.0);
	}
    else
        section_data = 0.0;

    return section_data / (3.0 * 3.14159265359 * pow(h, 2.0));
}


float ViscosityKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float r3h3 = - (r * r * r) / (2.0 * (h * h * h));
        float r2h2 = (r * r) / (h * h);
        float r1h1 = h / (2.0 * r);
        return 15.0 / (3.14159265359 * pow(h, 3.0)) * (r3h3 + r2h2 + r1h1 - 1);
    } else {
        return 0.0;
    }
}

void main() {
    uint id = gl_GlobalInvocationID.x;
    if(id >= positions.length()) {
		return;
	}

    float density_sum = 0.f;

    // Init settings
    float h = smoothing_length;

    // Loop over neighboring cells
    for (uint i = 0; i < positions.length(); ++i) {
            vec3 delta = positions[id].xyz - positions[i].xyz;
            float r = distance(positions[id].xyz, positions[i].xyz);
//            float r = length(delta);
			if (r < h) {
			    density_sum += particle_mass * /* poly6 kernel */ 315.f * pow(h * h - r * r, 3) / (64.f * PI_FLOAT * pow(h, 9));
            }
    }

    density_sum = min(density_sum, 5000.f);
    float pressure = max(u_gas_constant * (density_sum - u_density_0), 0.f);


    positions[id].w = density_sum;
    velocities[id].w = pressure;
}


)";





/****************************** ComputeCMUForce *******************************/


const char* ComputeCMUForce_cs = R"(

#version 460

layout (local_size_x = 256) in;

#define PI_FLOAT 3.1415927410125732421875f
layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[]; // position xyz, density w
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, pressure w
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[]; // acceleration xyz, dummy w
};

uniform float u_particle_radius;
uniform float u_particle_mass = 1.0f;
uniform float smoothing_length = 1.0f;

//uniform float u_bulk = 1000f;
uniform float u_viscosity = 3000.0f;
uniform float u_density_0 = 1000.0f;
uniform float u_gravity = 9.8f;

// Gradient of the spiky kernel
float SpikyKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h - r);
        return 15.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr * hr;
    } else {
        return 0.0;
    }
}

float ViscosityKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float r3h3 = - (r * r * r) / (2.0 * (h * h * h));
        float r2h2 = (r * r) / (h * h);
        float r1h1 = h / (2.0 * r);
        return 15.0 / (3.14159265359 * pow(h, 3.0)) * (r3h3 + r2h2 + r1h1 - 1);
    } else {
        return 0.0;
    }
}

// https://arxiv.org/pdf/1706.10263 page 7/26
float KernelFunction(float r, float h)
{
    float s = r / h;
    float section_data = 0.0;

    if ( s >= 0.0 && s < 1.0) 
    {
		section_data =  s * s * s - 6 * s + 6;
	} 
    else if (s >= 1.0 && s < 2.0) {
		section_data =  pow((2-s), 3.0);
	}
    else
        section_data = 0.0;

    return section_data / (3.0 * 3.14159265359 * pow(h, 2.0));
}

void main() {
    uint id = gl_GlobalInvocationID.x;

    vec3 pos = positions[id].xyz;
//    accelerations[id].xyz = vec3(0.0, 0.0, 0.0);
    const float h = smoothing_length;

    const float viscosity = u_viscosity;
    const float mass = u_particle_mass;

    // Compute Density was done in another compute shader
    vec3 force = vec3(0.0, 0.0, 0.0); // gravity

    const vec3 gravity = vec3(0.0, u_gravity, 0.0);

    // Loop over neighboring cells


    vec3 pressure_force = vec3(0.0, 0.0, 0.0);
    vec3 viscosity_force = vec3(0.0, 0.0, 0.0);

    //  calculate pressure force
    for (uint i = 0; i < positions.length(); ++i) {
        if (i != id) 
        {
            vec3 delta = pos - positions[i].xyz;
            float r = length(delta);
            if(r == 0.0)
				continue;
			if (r < h) {
                pressure_force -= mass * (velocities[id].w + velocities[i].w) / (2.0f * positions[i].w) *
                // gradient of spiky kernel
                    -45.f / (PI_FLOAT * pow(h, 6.0)) * pow(h - r, 2.0) * normalize(delta);

                viscosity_force += mass * (velocities[i].xyz - velocities[id].xyz) / positions[i].w *  
            // Laplacian of viscosity kernel
                45.f / (PI_FLOAT * pow(h, 6.0)) * (h - r);
            }
        }
    }
    viscosity_force *= viscosity;

    vec3 external_force =  -positions[id].w * gravity;

    //force = pressure_force  + external_force;
    force = pressure_force + viscosity_force + external_force;
    accelerations[id].xyz =  force / positions[id].w;
}


)";





/******************************* ComputeCMUPost *******************************/


const char* ComputeCMUPost_cs = R"(

#version 460

layout (local_size_x = 256) in;

layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[];
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[];
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[];
};

// vec3 : vertex, float : property. // But now property is not used.
// 3 vertex per triangle. only triangle use.
layout (std430, binding = 5) buffer CollisionMeshBuffer {
    vec4 collision_meshes[];
};

uniform float deltaTime;
uniform vec3 box_min;
uniform vec3 box_max;

uniform vec3 circle_center = vec3(0.0f);
uniform float circle_radius = 1.0f;
uniform float u_particle_radius = 0.1f;

bool CollidePositionTriangle(vec3 pos, vec3 vel, vec3 v0, vec3 v1, vec3 v2) {
    //  Trumbore intersection algorithm
    vec3 pos1 = pos;
    vec3 pos2 = pos + vel * deltaTime;

     vec3 d = pos2 - pos1;
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    vec3 h = cross(d, e2);
    float a = dot(e1, h);

    if (abs(a) < 0.00001) return false;
    float f = 1.0 / a;

    vec3 s = pos1 - v0;
    float u = f * dot(s, h);
    if (u < 0.0 || u > 1.0) return false;

    vec3 q = cross(s, e1);
    float v = f * dot(d, q);
    if (v < 0.0 || u + v > 1.0) return false;

    float t = f * dot(e2, q);

    return (t >= (0.0 - u_particle_radius) && t <= (1.0 + u_particle_radius));

}

void main() {
    uint id = gl_GlobalInvocationID.x;
    const float particle_size = u_particle_radius;

    vec3 pos = positions[id].xyz;
    vec3 vel = velocities[id].xyz;
    vec3 acc = accelerations[id].xyz;

    // Integrate motion


    // Reflect velocity vector when the particle hits the wall
    const float DAMP = 0.3f;



    
	// Collision with the circle, then bouns back
	//if the particle is inside the circle xyz
    vec3 circle_center_to_particle = positions[id].xyz - circle_center;

	if (dot(circle_center_to_particle, circle_center_to_particle) < circle_radius * circle_radius) {
		// if the particle is inside the circle, then move the particle to the surface of the circle
		pos.xyz = circle_center + normalize(circle_center_to_particle) * circle_radius;
		// reflect the velocity vector
		vel.xyz -= 2.0f * dot(vel.xyz, normalize(circle_center_to_particle)) * normalize(circle_center_to_particle) * 1.03f;
	}
	
    
    // Collision with the ground, then bounce back
    if (pos.y < 0.2 + particle_size) {
        pos.y = 0.2 + particle_size;
//         vel.y *= -1 * DAMP;
         //vel.y = - vel.y * DAMP;
//         acc.xyz += 128 * vec3(0, -1, 0) * vel.xyz;

        //p.velocity = p.velocity * -2 * dot(p.velocity, planeNormal) * planeNormal * bounceCoefficient
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 1, 0)) * vec3(0, 1, 0);

    }

    // Collision with the walls, then bounce back
    if (pos.x < box_min.x + particle_size) {
        pos.x = box_min.x + particle_size;

        //p.velocity = p.velocity * -2 * dot(p.velocity, planeNormal) * planeNormal * bounceCoefficient
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(1, 0, 0)) * vec3(1, 0, 0)* DAMP;

        // vel.x = - vel.x * DAMP;
    }
    if (pos.x > box_max.x - particle_size) {
        pos.x = box_max.x - particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(-1, 0, 0)) * vec3(-1, 0, 0)* DAMP;
//         vel.x = - vel.x * DAMP;
    }

    if (pos.y < box_min.y + particle_size) {
	    pos.y = box_min.y + particle_size;
		vel.y = - vel.y * DAMP;
	}
    if (pos.y > box_max.y - particle_size) {
        pos.y = box_max.y - particle_size;
		vel.y = - vel.y * DAMP;
	}

    if (pos.z < box_min.z + particle_size) {
        pos.z = box_min.z + particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 0, 1)) * vec3(0, 0, 1) * DAMP;
//        vel.z = - vel.z * DAMP;
    }
    if (pos.z > box_max.z - particle_size) {
        pos.z = box_max.z - particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 0, -1)) * vec3(0, 0, -1) * DAMP;
//        vel.z = - vel.z * DAMP;
    }

    vel.xyz += acc.xyz * deltaTime;
    
    // Triangle Check from collision_meshes
    for (uint collision_index = 0; collision_index< collision_meshes.length(); collision_index += 3) {

        // Get the vertices of the triangle
		vec3 v0 = collision_meshes[collision_index + 0].xyz;
		vec3 v1 = collision_meshes[collision_index + 1].xyz;
		vec3 v2 = collision_meshes[collision_index + 2].xyz;

        // If the position is in the triangle, then bounce back by the plane
		if(CollidePositionTriangle(pos.xyz, vel.xyz, v0, v1, v2))
        {
            // Calculate reflect vector. v = v - 2 * dot(v, n) * n. but the triangle is clockwise or not.
            pos = pos - vel * deltaTime;
            vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, normalize(cross(v1 - v0, v2 - v0))) * normalize(cross(v1 - v0, v2 - v0)) * DAMP;
		}
	}


    pos.xyz += vel.xyz * deltaTime;


    velocities[id].xyz = vel;
    positions[id].xyz = pos;
    
}


)";





/******************************* ComputeGridAcc *******************************/


const char* ComputeGridAcc_cs = R"(


#version 460

layout (local_size_x = 256) in;

#define PI_FLOAT 3.1415927410125732421875f
layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[]; // position xyz, density in w
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, density out w
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[]; // acceleration xyz, dummy w
};

layout (std430, binding = 3) buffer GridBuffer {
    int grid[];
};

layout (std430, binding = 4) buffer GridCountBuffer {
    int gridCount[];
};

uniform vec3 grid_size;
uniform ivec3 grid_resolution;
uniform vec3 grid_min_boundary; // Min boundary of the grid in world space
uniform uint particle_max_count_cell;

uniform float u_particle_radius;
uniform float u_particle_mass = 1.0f;
uniform float smoothing_length = 1.0f;

//uniform float u_bulk = 1000f;
uniform float u_viscosity = 3000.0f;
uniform float u_density_0 = 1000.0f;
uniform float u_gravity = 9.8f;



// Gradient of the spiky kernel
float SpikyKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h - r);
        return 15.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr * hr;
    } else {
        return 0.0;
    }
}

float ViscosityKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float r3h3 = - (r * r * r) / (2.0 * (h * h * h));
        float r2h2 = (r * r) / (h * h);
        float r1h1 = h / (2.0 * r);
        return 15.0 / (3.14159265359 * pow(h, 3.0)) * (r3h3 + r2h2 + r1h1 - 1);
    } else {
        return 0.0;
    }
}

// https://arxiv.org/pdf/1706.10263 page 7/26
float KernelFunction(float r, float h)
{
    float s = r / h;
    float section_data = 0.0;

    if ( s >= 0.0 && s < 1.0) 
    {
		section_data =  s * s * s - 6 * s + 6;
	} 
    else if (s >= 1.0 && s < 2.0) {
		section_data =  pow((2-s), 3.0);
	}
    else
        section_data = 0.0;

    return section_data / (3.0 * 3.14159265359 * pow(h, 2.0));
}

void main() {
    uint id = gl_GlobalInvocationID.x;
        if(id >= positions.length()) {
		return;
	}
    vec3 pos = positions[id].xyz;

    vec3 cell_size = grid_size / vec3(grid_resolution);

    ivec3 cell_index = ivec3(floor((pos - grid_min_boundary) / cell_size));

     const float h = smoothing_length;

    const float viscosity = u_viscosity;
    const float mass = u_particle_mass;

    // Compute Density was done in another compute shader
    vec3 force = vec3(0.0, 0.0, 0.0); // gravity

    const vec3 gravity = vec3(0.0, u_gravity, 0.0);

    // Loop over neighboring cells


    vec3 pressure_force = vec3(0.0, 0.0, 0.0);
    vec3 viscosity_force = vec3(0.0, 0.0, 0.0);


    // Loop over neighboring cells

    for (int z = -1; z <= 1; ++z) {
        for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {

                ivec3 neighborCellIndex = cell_index + ivec3(x, y, z);
                if (neighborCellIndex.x < 0 || neighborCellIndex.y < 0 || neighborCellIndex.z < 0 ||
                    neighborCellIndex.x >= grid_resolution.x || neighborCellIndex.y >= grid_resolution.y || neighborCellIndex.z >= grid_resolution.z) {
                    continue;
                }

                int flatIndex = neighborCellIndex.x + neighborCellIndex.y * int(grid_resolution.x) + neighborCellIndex.z * int(grid_resolution.x * grid_resolution.y);
//                int neighborCount = gridCount[flatIndex];
                int neighborCount = min(gridCount[flatIndex], int(particle_max_count_cell));

                for (int i = 0; i < neighborCount; ++i) {
                    int neighborIndex = grid[flatIndex * particle_max_count_cell + i];
                    if (neighborIndex == int(id)) continue;

                    vec3 neighborPos = positions[neighborIndex].xyz;

                    vec3 delta = pos - neighborPos;
                    float r = length(delta);
                    if(r == 0.0)
				        continue;
			        if (r < h) {
                        pressure_force -= mass * (velocities[id].w + velocities[neighborIndex].w) / (2.0f * positions[neighborIndex].w) *
                        // gradient of spiky kernel
                            -45.f / (PI_FLOAT * pow(h, 6.0)) * pow(h - r, 2.0) * normalize(delta);

                        viscosity_force += mass * (velocities[neighborIndex].xyz - velocities[id].xyz) / positions[neighborIndex].w *  
                    // Laplacian of viscosity kernel
                        45.f / (PI_FLOAT * pow(h, 6.0)) * (h - r);
                    }
                }
            }
        }
    }

//    const float rhoi = velocities[id].w;
//    for (uint i = 0; i < positions.length(); ++i) {
//        if (i != id) {
//            vec3 neighborPos = positions[i].xyz;
//            vec3 d = pos - neighborPos;
//
//            float d_length = length(d);
////            float r2 = max(dot(d, d), 0.00000001); // r * r
//
//            if(d_length < h)
//            {
//                const float rhoj = velocities[i].w; // Get density of neighbor
//                float q = d_length / h;
//                float u = 1.0 - q;
//                float w0 = C0 * u / rhoi / rhoj;
//                float wp = w0 * Cp * (rhoi + rhoj - 2 * density_0) * u / q;
//                float wv = w0 * Cv;
//                vec3 dv = velocities[id].xyz - velocities[i].xyz;
////
//                force += wv * dv + wp * d;
//
//            }
//        }
//    }

    viscosity_force *= viscosity;

    vec3 external_force =  -positions[id].w * gravity;

    //force = pressure_force  + external_force;
    force = pressure_force + viscosity_force + external_force;
    accelerations[id].xyz =  force / positions[id].w;
}


)";





/****************************** ComputeGridBatch ******************************/


const char* ComputeGridBatch_cs = R"(

#version 460

layout (local_size_x = 256) in;

layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[];
};

layout (std430, binding = 3) buffer GridBuffer {
    int grid[];
};

layout (std430, binding = 4) buffer GridCountBuffer {
    int gridCount[];
};

uniform vec3 grid_size;// = vec3(20, 60, 20); // Size of the grid in world space
uniform ivec3 grid_resolution;// = ivec3(10, 10, 10); // Number of cells in each dimension
uniform vec3 grid_min_boundary;// = vec3(-10, -30, -10);// Min boundary of the grid in world space
uniform uint particle_max_count_cell;// = 20;


void main() {
    uint id = gl_GlobalInvocationID.x;
    if(id >= positions.length()) {
		return;
	}
    vec3 pos = positions[id].xyz; 
    
    vec3 cell_size = grid_size / vec3(grid_resolution);

    ivec3 cell_index = ivec3(floor((pos - grid_min_boundary) / cell_size));

    int flat_index = cell_index.x + cell_index.y * grid_resolution.x + cell_index.z * (grid_resolution.x * grid_resolution.y);

    // Atomic add to ensure thread safety
    uint count = atomicAdd(gridCount[flat_index], 1);

    // Write particle index to grid
    if (count < particle_max_count_cell) {
        grid[flat_index * particle_max_count_cell + count] = int(id);
    }

}

)";





/***************************** ComputeGridDensity *****************************/


const char* ComputeGridDensity_cs = R"(

#version 460

layout (local_size_x = 256) in;

#define PI_FLOAT 3.1415927410125732421875f
layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[]; // position xyz, density w
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, pressure w
};

layout (std430, binding = 3) buffer GridBuffer {
    int grid[];
};

layout (std430, binding = 4) buffer GridCountBuffer {
    int gridCount[];
};

uniform vec3 grid_size;
uniform ivec3 grid_resolution;
uniform vec3 grid_min_boundary; // Min boundary of the grid in world space
uniform uint particle_max_count_cell;

uniform float particle_radius;
uniform float smoothing_length = 1.0f;
uniform float particle_mass = 1.0f;
uniform float u_density_0 = 1000.0f;
uniform float u_gas_constant = 2000.0f; // Gas constant for equation of state

// SPH kernel
float Poly6Kernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h * h - r * r);
        return 315.0 / (64.0 * 3.14159265359 * pow(h, 9.0)) * hr * hr * hr;
    } else {
        return 0.0;
    }
}

//// Gradient of the spiky kernel
//float SpikyKernel(float r, float h) {
//    if (r >= 0.0 && r <= h) {
//        float hr = (h - r);
//        return 15.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr * hr;
//    } else {
//        return 0.0;
//    }
//}

// https://arxiv.org/pdf/1706.10263 page 7/26
float KernelFunction(float r, float h)
{
    float s = r / h;
    float section_data = 0.0;

    if ( s >= 0.0 && s < 1.0) 
    {
		section_data =  s * s * s - 6 * s + 6;
	} 
    else if (s >= 1.0 && s < 2.0) {
		section_data =  pow((2-s), 3.0);
	}
    else
        section_data = 0.0;

    return section_data / (3.0 * 3.14159265359 * pow(h, 2.0));
}


float ViscosityKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float r3h3 = - (r * r * r) / (2.0 * (h * h * h));
        float r2h2 = (r * r) / (h * h);
        float r1h1 = h / (2.0 * r);
        return 15.0 / (3.14159265359 * pow(h, 3.0)) * (r3h3 + r2h2 + r1h1 - 1);
    } else {
        return 0.0;
    }
}

void main() {
    uint id = gl_GlobalInvocationID.x;
        if(id >= positions.length()) {
		return;
	}
    vec3 pos = positions[id].xyz;

    
    vec3 cell_size = grid_size / vec3(grid_resolution);

    ivec3 cell_index = ivec3(floor((pos - grid_min_boundary) / cell_size));

//    int flat_index = cell_index.x + cell_index.y * grid_resolution.x + cell_index.z * (grid_resolution.x * grid_resolution.y);


//    const float M_PI = 3.1415926535897932384626433832795;

//    // Init settings
//    float h = particle_radius;
//    float h2 = h * h;
//    float h8 = (h2 * h2) * (h2 * h2);
//    float C = 4 * particle_mass / (M_PI * h8);
    
    float density_sum = 0.f;


    // Init settings
    float h = smoothing_length;

    // add self density
	density_sum += particle_mass * /* poly6 kernel */ 315.f * pow( h * h, 3) / (64.f * PI_FLOAT * pow(h, 9));

    // Loop over neighboring cells
//    float density = u_density_0;
    for (int z = -1; z <= 1; ++z) {
        for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {

                ivec3 neighborCellIndex = cell_index + ivec3(x, y, z);
                if (neighborCellIndex.x < 0 || neighborCellIndex.y < 0 || neighborCellIndex.z < 0 ||
                    neighborCellIndex.x >= grid_resolution.x || neighborCellIndex.y >= grid_resolution.y || neighborCellIndex.z >= grid_resolution.z) {
                    continue;
                }

                int flatIndex = neighborCellIndex.x + neighborCellIndex.y * grid_resolution.x + neighborCellIndex.z *(grid_resolution.x * grid_resolution.y);
                int neighborCount = min(gridCount[flatIndex], int(particle_max_count_cell));

                for (int i = 0; i < neighborCount; ++i) {
                    int neighborIndex = grid[flatIndex * particle_max_count_cell + i];
                    if (neighborIndex == int(id)) continue;

                    vec3 neighborPos = positions[neighborIndex].xyz;
                    vec3 delta = positions[id].xyz - neighborPos;
                    float r = distance(positions[id].xyz, neighborPos);

			        if (r < h) {
			            density_sum += particle_mass * /* poly6 kernel */ 315.f * pow(h * h - r * r, 3) / (64.f * PI_FLOAT * pow(h, 9));
                    }
                }
            }
        }
    }


    density_sum = min(density_sum, 5000.f);
    float pressure = max(u_gas_constant * (density_sum - u_density_0), 0.f);


    positions[id].w = density_sum;
    velocities[id].w = pressure;
}


)";





/****************************** ComputeGridInit *******************************/


const char* ComputeGridInit_cs = R"(

#version 460
layout(local_size_x = 256) in; // Work group size

layout (std430, binding = 3) buffer GridBuffer {
    int grid[];
};

layout (std430, binding = 4) buffer GridCountBuffer {
    int gridCount[];
};

void main() {
    uint id = gl_GlobalInvocationID.x;

    // Initlize grid and gridCount
        if(id < grid.length()) {
    	grid[id] = -1;
	}

        if(id < gridCount.length()) {
	    gridCount[id] = 0;
    }
}

)";





/****************************** ComputeGridPost *******************************/


const char* ComputeGridPost_cs = R"(

#version 460

layout (local_size_x = 256) in;

layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[];
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[];
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[];
};

// vec3 : vertex, float : property. // But now property is not used.
// 3 vertex per triangle. only triangle use.
layout (std430, binding = 5) buffer CollisionMeshBuffer {
    vec4 collision_meshes[];
};

uniform float deltaTime;
uniform vec3 box_min;
uniform vec3 box_max;

uniform vec3 circle_center = vec3(0.0f);
uniform float circle_radius = 1.0f;
uniform float u_particle_radius = 0.1f;

bool CollidePositionTriangle(vec3 pos, vec3 vel, vec3 v0, vec3 v1, vec3 v2) {
    //  Trumbore intersection algorithm
    vec3 pos1 = pos;
    vec3 pos2 = pos + vel * deltaTime;

     vec3 d = pos2 - pos1;
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    vec3 h = cross(d, e2);
    float a = dot(e1, h);

    if (abs(a) < 0.00001) return false;
    float f = 1.0 / a;

    vec3 s = pos1 - v0;
    float u = f * dot(s, h);
    if (u < 0.0 || u > 1.0) return false;

    vec3 q = cross(s, e1);
    float v = f * dot(d, q);
    if (v < 0.0 || u + v > 1.0) return false;

    float t = f * dot(e2, q);

    return (t >= (0.0 - u_particle_radius) && t <= (1.0 + u_particle_radius));

}

void main() {
    uint id = gl_GlobalInvocationID.x;
    const float particle_size = u_particle_radius;

    vec3 pos = positions[id].xyz;
    vec3 vel = velocities[id].xyz;
    vec3 acc = accelerations[id].xyz;

    // Integrate motion


    // Reflect velocity vector when the particle hits the wall
    const float DAMP = 0.3f;



    
	// Collision with the circle, then bouns back
	//if the particle is inside the circle xyz
    vec3 circle_center_to_particle = positions[id].xyz - circle_center;

	if (dot(circle_center_to_particle, circle_center_to_particle) < circle_radius * circle_radius) {
		// if the particle is inside the circle, then move the particle to the surface of the circle
		pos.xyz = circle_center + normalize(circle_center_to_particle) * circle_radius;
		// reflect the velocity vector
		vel.xyz -= 2.0f * dot(vel.xyz, normalize(circle_center_to_particle)) * normalize(circle_center_to_particle) * 1.03f;
	}
	
    
    // Collision with the ground, then bounce back
    if (pos.y < 0.2 + particle_size) {
        pos.y = 0.2 + particle_size;
//         vel.y *= -1 * DAMP;
         //vel.y = - vel.y * DAMP;
//         acc.xyz += 128 * vec3(0, -1, 0) * vel.xyz;

        //p.velocity = p.velocity * -2 * dot(p.velocity, planeNormal) * planeNormal * bounceCoefficient
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 1, 0)) * vec3(0, 1, 0);

    }

    // Collision with the walls, then bounce back
    if (pos.x < box_min.x + particle_size) {
        pos.x = box_min.x + particle_size;

        //p.velocity = p.velocity * -2 * dot(p.velocity, planeNormal) * planeNormal * bounceCoefficient
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(1, 0, 0)) * vec3(1, 0, 0)* DAMP;

        // vel.x = - vel.x * DAMP;
    }
    if (pos.x > box_max.x - particle_size) {
        pos.x = box_max.x - particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(-1, 0, 0)) * vec3(-1, 0, 0)* DAMP;
//         vel.x = - vel.x * DAMP;
    }

    if (pos.y < box_min.y + particle_size) {
	    pos.y = box_min.y + particle_size;
		vel.y = - vel.y * DAMP;
	}
    if (pos.y > box_max.y - particle_size) {
        pos.y = box_max.y - particle_size;
		vel.y = - vel.y * DAMP;
	}

    if (pos.z < box_min.z + particle_size) {
        pos.z = box_min.z + particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 0, 1)) * vec3(0, 0, 1) * DAMP;
//        vel.z = - vel.z * DAMP;
    }
    if (pos.z > box_max.z - particle_size) {
        pos.z = box_max.z - particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 0, -1)) * vec3(0, 0, -1) * DAMP;
//        vel.z = - vel.z * DAMP;
    }

    vel.xyz += acc.xyz * deltaTime;
    
    // Triangle Check from collision_meshes
    for (uint collision_index = 0; collision_index< collision_meshes.length(); collision_index += 3) {

        // Get the vertices of the triangle
		vec3 v0 = collision_meshes[collision_index + 0].xyz;
		vec3 v1 = collision_meshes[collision_index + 1].xyz;
		vec3 v2 = collision_meshes[collision_index + 2].xyz;

        // If the position is in the triangle, then bounce back by the plane
		if(CollidePositionTriangle(pos.xyz, vel.xyz, v0, v1, v2))
        {
            // Calculate reflect vector. v = v - 2 * dot(v, n) * n. but the triangle is clockwise or not.
            pos = pos - vel * deltaTime;
            vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, normalize(cross(v1 - v0, v2 - v0))) * normalize(cross(v1 - v0, v2 - v0)) * DAMP;
		}
	}


    pos.xyz += vel.xyz * deltaTime;


    velocities[id].xyz = vel;
    positions[id].xyz = pos;
    
}



)";





/******************************* ComputeGridSPH *******************************/


const char* ComputeGridSPH_cs = R"(

#version 460

layout (local_size_x = 1024) in;

layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[]; // position xyz, density in w
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, density out w
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[]; // acceleration xyz, dummy w
};

layout (std430, binding = 3) buffer GridBuffer {
    int grid[];
};

layout (std430, binding = 4) buffer GridCountBuffer {
    int gridCount[];
};

uniform vec3 gridSize;
uniform ivec3 gridResolution;
uniform float smoothingLength;
uniform vec3 grid_min_boundary; // Min boundary of the grid in world space
uniform uint particle_max_count_cell;

uniform float particle_radius = 0.1f;
uniform float collision_stiffness = 100.0f; // Stiffness of the collision response
uniform float deltaTime;

void main() {
    uint id = gl_GlobalInvocationID.x;
        if(id >= positions.length()) {
		return;
	}
    vec3 pos = positions[id].xyz;
    vec4 vel = velocities[id];

    vec3 cellSize = gridSize / gridResolution;

    vec3 normalizedPos = (pos - grid_min_boundary) / gridSize; // vec3  0~1 
    vec3 grid_resolution = vec3(gridResolution);
    ivec3 cellIndex = ivec3(floor(normalizedPos * ivec3(100) / grid_resolution));

    /*
    // Compute grid cell index
    cellIndex = clamp(cellIndex, ivec3(0), gridResolution - ivec3(1));
    ivec3 cellIndex = ivec3(floor(pos / gridSize * gridResolution));
    */

    // Loop over neighboring cells
    vec3 force = vec3(0.0);
    for (int z = -1; z <= 1; ++z) {
        for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {

                ivec3 neighborCellIndex = cellIndex + ivec3(x, y, z);
                if (neighborCellIndex.x < 0 || neighborCellIndex.y < 0 || neighborCellIndex.z < 0 ||
                    neighborCellIndex.x >= gridResolution.x || neighborCellIndex.y >= gridResolution.y || neighborCellIndex.z >= gridResolution.z) {
                    continue;
                }

                int flatIndex = neighborCellIndex.x + neighborCellIndex.y * int(gridResolution.x) + neighborCellIndex.z * int(gridResolution.x * gridResolution.y);
                int neighborCount = gridCount[flatIndex];

                for (int i = 0; i < neighborCount; ++i) {
                    int neighborIndex = grid[flatIndex * particle_max_count_cell + i];
                    if (neighborIndex == int(id)) continue;

                    vec3 neighborPos = positions[neighborIndex].xyz;
                    vec3 r = pos - neighborPos;
                    float rLen = length(r);


                    if (rLen < smoothingLength) {
                        float q = rLen / smoothingLength;
                        force += (1.0 - q) * normalize(r);

                    }
                }
            }
        }
    }

    accelerations[id] = vec4(force, 0.0);
}


)";





/****************************** ComputeParticles ******************************/


const char* ComputeParticles_cs = R"(

#version 460

layout(local_size_x = 256) in; // Work group size

layout(std430, binding = 0) buffer PosBuffer {
    vec4 positions[];
};

layout(std430, binding = 1) buffer VelBuffer {
    vec4 velocities[];
};

layout(std430, binding = 2) buffer AccBuffer {
    vec4 accelerations[];
};

uniform float deltaTime;


uniform vec3 box_min;
uniform vec3 box_max;

uniform vec3 circle_center = vec3(0.0f);
uniform float circle_radius = 1.0f;

void main() {
    uint id = gl_GlobalInvocationID.x;

    // Update  particle velocity
    velocities[id] += accelerations[id]* deltaTime;

    // Update particle position
    positions[id] += velocities[id] * deltaTime;

	// Collision with the circle, then bouns back
	// if the particle is inside the circle xyz
	vec3 circle_center_to_particle = positions[id].xyz - circle_center;

	if (dot(circle_center_to_particle, circle_center_to_particle) < circle_radius * circle_radius) {
		// if the particle is inside the circle, then move the particle to the surface of the circle
		positions[id].xyz = circle_center + normalize(circle_center_to_particle) * circle_radius;
		// reflect the velocity vector
		velocities[id].xyz -= 2.0f * dot(velocities[id].xyz, normalize(circle_center_to_particle)) * normalize(circle_center_to_particle);
	}
	

    // Collision with the ground, then bouns back
    if (positions[id].y < 0.0) {
		positions[id].y = 0.0;
		velocities[id].y = -velocities[id].y;
	}
    // Collision with the walls, then bouns back
	if (positions[id].x < box_min.x) {
		positions[id].x = box_min.x;
		velocities[id].x = -velocities[id].x;
	}
    if (positions[id].x > box_max.x) {
		positions[id].x = box_max.x;
		velocities[id].x = -velocities[id].x;
	}

	if (positions[id].z < box_min.z) {
		positions[id].z = box_min.z;
		velocities[id].z = -velocities[id].z;
	}
	if (positions[id].z > box_max.z) {
		positions[id].z = box_max.z;
		velocities[id].z = -velocities[id].z;
	}
}


)";





/**************************** ComputeParticlesSPH *****************************/


const char* ComputeParticlesSPH_cs = R"(

#version 460

layout(local_size_x = 256) in; // Work group size

layout(std430, binding = 0) buffer PosBuffer {
    vec4 positions[];
};

layout(std430, binding = 1) buffer VelBuffer {
    vec4 velocities[];
};

layout(std430, binding = 2) buffer AccBuffer {
    vec4 accelerations[];
};

uniform float deltaTime;


uniform vec3 box_min;
uniform vec3 box_max;

uniform vec3 circle_center = vec3(0.0f);
uniform float circle_radius = 1.0f;

uniform float particle_radius = 0.1f;
uniform float collision_stiffness = 100.0f; // Stiffness of the collision response

void main() {
    uint id = gl_GlobalInvocationID.x;

    // Update  particle velocity
    velocities[id] += accelerations[id]* deltaTime;

    // Update particle position
    positions[id] += velocities[id] * deltaTime;


	// SPH: Particle-particle collision
    vec3 pos = positions[id].xyz;

    // Check collision with other particles by Brute force method
    for (uint i = 0; i < positions.length(); ++i) {
        if (i != id) {
            vec3 other_pos = positions[i].xyz;
            vec3 diff = pos - other_pos;
            float dist = length(diff);
            float combined_radius = 2.0 * particle_radius;

            if (dist < combined_radius) {
                vec3 direction = normalize(diff);
                float penetration = combined_radius - dist;
                vec3 correction = direction * penetration * collision_stiffness * deltaTime;

                // Apply corrections to positions and velocities
                positions[id].xyz += correction * 0.5;
                positions[i].xyz -= correction * 0.5;

                velocities[id].xyz += correction * 0.5;
                velocities[i].xyz -= correction * 0.5;
            }
        }
    }




	// Collision with the circle, then bouns back
	// if the particle is inside the circle xyz
	vec3 circle_center_to_particle = positions[id].xyz - circle_center;

	if (dot(circle_center_to_particle, circle_center_to_particle) < circle_radius * circle_radius) {
		// if the particle is inside the circle, then move the particle to the surface of the circle
		positions[id].xyz = circle_center + normalize(circle_center_to_particle) * circle_radius;
		// reflect the velocity vector
		velocities[id].xyz -= 2.0f * dot(velocities[id].xyz, normalize(circle_center_to_particle)) * normalize(circle_center_to_particle) * 1.03f;
	}
	

    // Collision with the ground, then bouns back
    if (positions[id].y < 0.2) {
		positions[id].y = 0.2;
		velocities[id].y = -velocities[id].y * 0.5;
	}
    // Collision with the walls, then bouns back
	if (positions[id].x < box_min.x) {
		positions[id].x = box_min.x;
		velocities[id].x = -velocities[id].x * 0.7;
	}
    if (positions[id].x > box_max.x) {
		positions[id].x = box_max.x;
		velocities[id].x = -velocities[id].x * 0.7;
	}

	if (positions[id].z < box_min.z) {
		positions[id].z = box_min.z;
		velocities[id].z = -velocities[id].z * 0.7;
	}
	if (positions[id].z > box_max.z) {
		positions[id].z = box_max.z;
		velocities[id].z = -velocities[id].z * 0.7;
	}
}


)";





/************************** ComputeParticleTriBuffer **************************/


const char* ComputeParticleTriBuffer_cs = R"(

#version 460

layout(local_size_x = 1024) in; // Work group size

layout(std430, binding = 0) buffer PositionIn {
    vec4 positions_in[]; // xyz: position, w: density
};

layout(std430, binding = 1) buffer PositionOut {
    vec4 positions_out[]; // xyz: position, w: density
};

layout(std430, binding = 2) buffer VelocityIn {
    vec4 velocities_in[]; // xyz: velocity, w: pressure
};

layout(std430, binding = 3) buffer VelocityOut{
    vec4 velocities_out[]; // xyz: velocity, w: pressure
};

layout(std430, binding = 4) buffer AccelerationIn {
    vec4 accelerations_in[]; // xyz: acceleration, w: unused
};

layout(std430, binding = 5) buffer AccelerationOut{
    vec4 accelerations_out[]; // xyz: acceleration, w: unused
};

uniform float deltaTime;
uniform vec3 box_min;
uniform vec3 box_max;

uniform vec3 circle_center = vec3(0.0f);
uniform float circle_radius = 1.0f;

uniform float particle_radius = 0.1f;
uniform float collision_stiffness = 100.0f; // Stiffness of the collision response

uniform float rest_density = 1000.0f; // Rest density of the fluid
uniform float gas_constant = 2000.0f; // Gas constant for equation of state
uniform float h = 0.15f; // Smoothing radius

uniform vec3 gravity = vec3(0.0, -9.8, 0.0); // Gravity acceleration
uniform float max_velocity = 10.0f; // Maximum allowed velocity
uniform bool is_play = false;

// SPH kernels
float poly6_kernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h * h - r * r);
        return 315.0 / (64.0 * 3.14159265359 * pow(h, 9.0)) * hr * hr * hr;
    } else {
        return 0.0;
    }
}

// Gradient of the spiky kernel
float spiky_kernel_gradient(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h - r);
        return -45.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr;
    } else {
        return 0.0;
    }
}

void main() {
    uint id = gl_GlobalInvocationID.x;

    // Compute density
    float density = 0.0;
    float density_final = 0.0;

    // Temp datas from the input buffers
    vec3 pos = positions_in[id].xyz;
    vec3 vel = velocities_in[id].xyz;
    vec3 acc = accelerations_in[id].xyz;

    // Initialize acceleration with gravity
    accelerations_out[id].xyz = gravity;
    //acc = gravity;

    // Apply pressure forces
    vec3 pressure_force = vec3(0.0);
    
    vel.xyz += gravity * deltaTime;
    pos.xyz += vel.xyz * deltaTime;
    
    positions_out[id].xyz = pos;
    velocities_out[id].xyz = vel;
    accelerations_out[id].xyz = acc;



    barrier(); // Ensure all updates are completed before proceeding

    // SPH: Particle-particle collision
    for (uint i = 0; i < positions_in.length(); ++i) {
        if (i != id) {
            vec3 other_pos = positions_in[i].xyz;
            vec3 diff = pos - other_pos;
            float dist = length(diff);
            float combined_radius = 2.0 * particle_radius;

            if(dist < combined_radius)
            {
                vec3 direction = normalize(diff);
                float penetration = combined_radius - dist;
                vec3 correction = penetration * direction * collision_stiffness * deltaTime;

                // 

                // Apply corrections to positions and velocities
                positions_out[id].xyz += correction * 0.5;
                positions_out[i].xyz -= correction * 0.5;

                velocities_out[id].xyz += correction * 0.5;
                velocities_out[i].xyz -= correction * 0.5;
            }
        }
    }
     
    
    barrier(); // Ensure all updates are completed before proceeding


    // Collision with the circle, then bounce back
    if(is_play)
    {
        vec3 circle_center_to_particle = pos.xyz - circle_center;

        if (dot(circle_center_to_particle, circle_center_to_particle) < circle_radius * circle_radius) {
            // If the particle is inside the circle, then move the particle to the surface of the circle
            positions_out[id].xyz = circle_center + normalize(circle_center_to_particle) * circle_radius;
            // Reflect the velocity vector
            velocities_out[id].xyz -= 2.0f * dot( velocities_out[id].xyz, normalize(circle_center_to_particle)) * normalize(circle_center_to_particle);
        }
    }
    
    // Under codes should be fixed with the boundary condition.

    // Collision with the ground, then bounce back
    if (positions_out[id].y < 0.2) {
        positions_out[id].y = 0.2;
         velocities_out[id].y = - velocities_out[id].y * 0.9f;
    }

    // Collision with the walls, then bounce back
    if (positions_out[id].x < box_min.x) {
        positions_out[id].x = box_min.x;
         velocities_out[id].x = - velocities_out[id].x * 0.9f;
    }
    if (positions_out[id].x > box_max.x) {
        positions_out[id].x = box_max.x;
         velocities_out[id].x = - velocities_out[id].x * 0.9f;
    }

    if (positions_out[id].y < box_min.y) {
	    positions_out[id].y = box_min.y;
		velocities_out[id].y = - velocities_out[id].y * 0.9f;
	}
    if (positions_out[id].y > box_max.y) {
        positions_out[id].y = box_max.y;
		velocities_out[id].y = - velocities_out[id].y * 0.9f;
	}



    if (positions_out[id].z < box_min.z) {
        positions_out[id].z = box_min.z;
        velocities_out[id].z = - velocities_out[id].z * 0.9f;
    }
    if (positions_out[id].z > box_max.z) {
        positions_out[id].z = box_max.z;
        velocities_out[id].z = - velocities_out[id].z * 0.9f;
    }
    

//    positions_out[id].xyz = pos;
//    velocities_out[id].xyz = vel;
//    accelerations_out[id].xyz = acc;
}


)";





/**************************** ComputeSPH_pressure *****************************/


const char* ComputeSPH_pressure_cs = R"(

//#version 430 core
//
//layout(local_size_x = 256) in; // Work group size
//
//layout(std430, binding = 0) buffer PosBuffer {
//    vec4 positions[]; // xyz: position, w: density
//};
//
//layout(std430, binding = 1) buffer VelBuffer {
//    vec4 velocities[]; // xyz: velocity, w: pressure
//};
//
//layout(std430, binding = 2) buffer AccBuffer {
//    vec4 accelerations[]; // xyz: acceleration, w: unused
//};
//
//uniform float deltaTime;
//uniform vec3 box_min;
//uniform vec3 box_max;
//
//uniform vec3 circle_center = vec3(0.0f);
//uniform float circle_radius = 1.0f;
//
//uniform float particle_radius = 0.1f;
//uniform float collision_stiffness = 100.0f; // Stiffness of the collision response
//
//uniform float rest_density = 1000.0f; // Rest density of the fluid
//uniform float gas_constant = 2000.0f; // Gas constant for equation of state
//uniform float h = 0.05f; // Smoothing radius
//
//uniform vec3 gravity = vec3(0.0, -9.8, 0.0); // Gravity acceleration
//uniform float max_velocity = 10.0f; // Maximum allowed velocity
//
//// SPH kernels
//float poly6_kernel(float r, float h) {
//    if (r >= 0.0 && r <= h) {
//        float hr = (h * h - r * r);
//        return 315.0 / (64.0 * 3.14159265359 * pow(h, 9.0)) * hr * hr * hr;
//    } else {
//        return 0.0;
//    }
//}
//
//// Gradient of the spiky kernel
//float spiky_kernel_gradient(float r, float h) {
//    if (r >= 0.0 && r <= h) {
//        float hr = (h - r);
//        return -45.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr;
//    } else {
//        return 0.0;
//    }
//}
//
//void main() {
//    uint id = gl_GlobalInvocationID.x;
//
//    // Compute density
//    float density = 0.0;
//    vec3 pos = positions[id].xyz;
//
//    for (uint i = 0; i < positions.length(); ++i) {
//        if (i != id) {
//            vec3 other_pos = positions[i].xyz;
//            float dist = length(pos - other_pos);
//            density += poly6_kernel(dist, h);
//        }
//    }
//    positions[id].w = density * rest_density;
//
//    barrier(); // Ensure all densities are computed before proceeding
//
//    // Compute pressure
//    velocities[id].w = gas_constant * (positions[id].w - rest_density);
//
//    barrier(); // Ensure all pressures are computed before proceeding
//
//    // Initialize acceleration with gravity
//    accelerations[id].xyz = gravity;
//
//    // Apply pressure forces
//    vec3 pressure_force = vec3(0.0);
//
//    for (uint i = 0; i < positions.length(); ++i) {
//        if (i != id) {
//            vec3 other_pos = positions[i].xyz;
//            vec3 diff = pos - other_pos;
//            float dist = length(diff);
//            if (dist < h) {
//                vec3 gradient = spiky_kernel_gradient(dist, h) * normalize(diff);
//                pressure_force -= (velocities[id].w + velocities[i].w) / (2.0 * positions[i].w) * gradient;
//            }
//        }
//    }
//    if (positions[id].w != 0.0) {
//        accelerations[id].xyz += pressure_force / positions[id].w;
//    }
//
//    // Apply damping to stabilize the particles
//    accelerations[id].xyz *= 0.5;
//
//    // Update particle velocity
//    velocities[id].xyz += accelerations[id].xyz * deltaTime;
//
//    // Limit velocity to prevent excessive speeds
//    if (length(velocities[id].xyz) > max_velocity) {
//        velocities[id].xyz = normalize(velocities[id].xyz) * max_velocity;
//    }
//
//    // Update particle position
//    positions[id].xyz += velocities[id].xyz * deltaTime;
//
//    barrier(); // Ensure all updates are completed before proceeding
//
//    // SPH: Particle-particle collision
//    for (uint i = 0; i < positions.length(); ++i) {
//        if (i != id) {
//            vec3 other_pos = positions[i].xyz;
//            vec3 diff = pos - other_pos;
//            float dist = length(diff);
//            float combined_radius = 2.0 * particle_radius;
//
//            if (dist < combined_radius) {
//                vec3 direction = normalize(diff);
//                float penetration = combined_radius - dist;
//                vec3 correction = direction * penetration * collision_stiffness * deltaTime;
//
//                // Apply corrections to positions and velocities
//                positions[id].xyz += correction * 0.5;
//                positions[i].xyz -= correction * 0.5;
//
//                velocities[id].xyz += correction * 0.5;
//                velocities[i].xyz -= correction * 0.5;
//            }
//        }
//    }
//
//    // Collision with the circle, then bounce back
//    vec3 circle_center_to_particle = positions[id].xyz - circle_center;
//
//    if (dot(circle_center_to_particle, circle_center_to_particle) < circle_radius * circle_radius) {
//        // If the particle is inside the circle, then move the particle to the surface of the circle
//        positions[id].xyz = circle_center + normalize(circle_center_to_particle) * circle_radius;
//        // Reflect the velocity vector
//        velocities[id].xyz -= 2.0f * dot(velocities[id].xyz, normalize(circle_center_to_particle)) * normalize(circle_center_to_particle);
//    }
//
//    // Collision with the ground, then bounce back
//    if (positions[id].y < 0.2) {
//        positions[id].y = 0.2;
//        velocities[id].y = -velocities[id].y * 0.5f;
//    }
//
//    // Collision with the walls, then bounce back
//    if (positions[id].x < box_min.x) {
//        positions[id].x = box_min.x;
//        velocities[id].x = -velocities[id].x * 0.5f;
//    }
//    if (positions[id].x > box_max.x) {
//        positions[id].x = box_max.x;
//        velocities[id].x = -velocities[id].x * 0.5f;
//    }
//
//    if (positions[id].z < box_min.z) {
//        positions[id].z = box_min.z;
//        velocities[id].z = -velocities[id].z * 0.5f;
//    }
//    if (positions[id].z > box_max.z) {
//        positions[id].z = box_max.z;
//        velocities[id].z = -velocities[id].z * 0.5f;
//    }
//}
//


#version 460

layout(local_size_x = 256) in; // Work group size

layout(std430, binding = 0) buffer PosBuffer {
    vec4 positions[]; // xyz: position, w: density
};

layout(std430, binding = 1) buffer VelBuffer {
    vec4 velocities[]; // xyz: velocity, w: pressure
};

layout(std430, binding = 2) buffer AccBuffer {
    vec4 accelerations[]; // xyz: acceleration, w: unused
};

uniform float deltaTime;
uniform vec3 box_min;
uniform vec3 box_max;

uniform vec3 circle_center = vec3(0.0f);
uniform float circle_radius = 1.0f;

uniform float particle_radius = 0.1f;
uniform float collision_stiffness = 100.0f; // Stiffness of the collision response

uniform float rest_density = 1000.0f; // Rest density of the fluid
uniform float gas_constant = 2000.0f; // Gas constant for equation of state
uniform float h = 0.15f; // Smoothing radius

uniform vec3 gravity = vec3(0.0, -9.8, 0.0); // Gravity acceleration
uniform float max_velocity = 10.0f; // Maximum allowed velocity
uniform bool is_play = false;

// SPH kernels
float poly6_kernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h * h - r * r);
        return 315.0 / (64.0 * 3.14159265359 * pow(h, 9.0)) * hr * hr * hr;
    } else {
        return 0.0;
    }
}

// Gradient of the spiky kernel
float spiky_kernel_gradient(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h - r);
        return -45.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr;
    } else {
        return 0.0;
    }
}

void main() {
    uint id = gl_GlobalInvocationID.x;

    // Compute density
    float density = 0.0;
    vec3 pos = positions[id].xyz;

//    for (uint i = 0; i < positions.length(); ++i) {
//        if (i != id) {
//            vec3 other_pos = positions[i].xyz;
//            float dist = length(pos - other_pos);
//            density += poly6_kernel(dist, h);
//        }
//    }
//    positions[id].w = density * rest_density;

//    barrier(); // Ensure all densities are computed before proceeding

    // Compute pressure
//    velocities[id].w = gas_constant * (positions[id].w - rest_density);

//    barrier(); // Ensure all pressures are computed before proceeding

    // Initialize acceleration with gravity
    accelerations[id].xyz = gravity;

    // Apply pressure forces
    vec3 pressure_force = vec3(0.0);

//    for (uint i = 0; i < positions.length(); ++i) {
//        if (i != id) {
//            vec3 other_pos = positions[i].xyz;
//            vec3 diff = pos - other_pos;
//            float dist = length(diff);
//            if (dist < h) {
//                vec3 gradient = spiky_kernel_gradient(dist, h) * normalize(diff);
//                pressure_force -= (velocities[id].w + velocities[i].w) / (2.0 * positions[i].w) * gradient;
//            }
//        }
//    }
//    if (positions[id].w != 0.0) {
//        accelerations[id].xyz += pressure_force / positions[id].w;
//    }

    // Apply damping to stabilize the particles
//accelerations[id].xyz *= 0.5;

    // Update particle velocity
    velocities[id].xyz += accelerations[id].xyz * deltaTime;

    // Limit velocity to prevent excessive speeds
//    if (length(velocities[id].xyz) > max_velocity) {
//        velocities[id].xyz = normalize(velocities[id].xyz) * max_velocity;
//    }

    // Update particle position
    positions[id].xyz += velocities[id].xyz * deltaTime;

    barrier(); // Ensure all updates are completed before proceeding

    // SPH: Particle-particle collision
    for (uint i = 0; i < positions.length(); ++i) {
        if (i != id) {
            vec3 other_pos = positions[i].xyz;
            vec3 diff = pos - other_pos;
            float dist = length(diff);
            float combined_radius = 2.0 * particle_radius;

            if(dist < combined_radius)
            {
                density += poly6_kernel(dist, h);
                positions[id].w = density * rest_density;
                velocities[id].w = gas_constant * (positions[id].w - rest_density);

            
                if (dist < h) {
                    vec3 gradient = spiky_kernel_gradient(dist, h) * normalize(diff);
                    pressure_force -= (velocities[id].w + velocities[i].w) / (2.0 * positions[i].w) * gradient;
                }
                if (positions[id].w != 0.0) {
                    accelerations[id].xyz += pressure_force / positions[id].w;
                }

            vec3 direction = normalize(diff);
            float penetration = combined_radius - dist;
            vec3 correction = penetration * direction * collision_stiffness * deltaTime;

            // Apply corrections to positions and velocities
            positions[id].xyz += correction * 0.5;
            positions[i].xyz -= correction * 0.5;

            velocities[id].xyz += correction * 0.5;
            velocities[i].xyz -= correction * 0.5;
            }
        }
    }

    // Collision with the circle, then bounce back
    if(is_play)
    {
        vec3 circle_center_to_particle = positions[id].xyz - circle_center;

        if (dot(circle_center_to_particle, circle_center_to_particle) < circle_radius * circle_radius) {
            // If the particle is inside the circle, then move the particle to the surface of the circle
            positions[id].xyz = circle_center + normalize(circle_center_to_particle) * circle_radius;
            // Reflect the velocity vector
            velocities[id].xyz -= 2.0f * dot(velocities[id].xyz, normalize(circle_center_to_particle)) * normalize(circle_center_to_particle);
        }
    }
    
    // Collision with the ground, then bounce back
    if (positions[id].y < 0.2) {
        positions[id].y = 0.2;
        velocities[id].y = -velocities[id].y * 0.9f;
    }

    // Collision with the walls, then bounce back
    if (positions[id].x < box_min.x) {
        positions[id].x = box_min.x;
        velocities[id].x = -velocities[id].x * 0.9f;
    }
    if (positions[id].x > box_max.x) {
        positions[id].x = box_max.x;
        velocities[id].x = -velocities[id].x * 0.9f;
    }

    if(positions[id].y < box_min.y)
	{
		positions[id].y = box_min.y;
		velocities[id].y = -velocities[id].y * 0.9f;
	}
    if(positions[id].y > box_max.y)
    {
        positions[id].y = box_max.y;
		velocities[id].y = -velocities[id].y * 0.9f;
	}



    if (positions[id].z < box_min.z) {
        positions[id].z = box_min.z;
        velocities[id].z = -velocities[id].z * 0.9f;
    }
    if (positions[id].z > box_max.z) {
        positions[id].z = box_max.z;
        velocities[id].z = -velocities[id].z * 0.9f;
    }
}


)";





/******************************** ComputeTrace ********************************/


const char* ComputeTrace_cs = R"(



)";





/********************************** default ***********************************/


const char* default_fs = R"(

#version 460
out vec4 FragColor;

in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}

)";



const char* default_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}

)";





/********************************* DefaultMVP *********************************/


const char* DefaultMVP_fs = R"(

#version 460
out vec4 FragColor;

uniform vec3 color = vec3(1.0);

void main()
{
    FragColor = vec4(vec3(color), 1.0f);
}

)";



const char* DefaultMVP_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

)";





/******************************** DefaultMVPL *********************************/


const char* DefaultMVPL_fs = R"(

#version 460
out vec4 FragColor;

in float _diffuse;
uniform vec3 color;

void main()
{
    float ambient = 0.5f;
    
    FragColor = clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * vec4(vec3(color), 1.0f);
}

)";



const char* DefaultMVPL_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform vec3 light_dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float _diffuse;
void main()
{
    vec3 Worldnormal;
    vec3 worldLightPos;
    vec3 mlight_Dir = vec3(0, 0, 0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Worldnormal = mat3(model) * aNormal;
    
    mlight_Dir = normalize(light_dir);
    Worldnormal = normalize(Worldnormal);
    float diff = max(dot(Worldnormal, -mlight_Dir), 0.0);

    _diffuse = diff;
}



)";





/******************************** DefaultMVPLT ********************************/


const char* DefaultMVPLT_fs = R"(

#version 460
out vec4 FragColor;

in float _diffuse;
in vec2 TexCoords;
uniform vec3 color;
uniform sampler2D tex;
void main()
{
    float ambient = 0.5f;
    vec2 texcoord_flipy = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec4 tex_color = texture(tex, texcoord_flipy);
            if(tex_color.a < 0.1) 
            discard;
    
    FragColor = clamp(ambient + _diffuse*0.25f, 0.0, 1.0) * tex_color * vec4(color, 1.0f);
}

)";



const char* DefaultMVPLT_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform vec3 light_dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out float _diffuse;
void main()
{
    vec3 Worldnormal;
    vec3 worldLightPos;
    vec3 mlight_Dir = vec3(0, 0, 0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Worldnormal = mat3(model) * aNormal;
    
    mlight_Dir = normalize(light_dir);
    Worldnormal = normalize(Worldnormal);
    float diff = max(dot(Worldnormal, -mlight_Dir), 0.0);
    TexCoords = aTexCoords;
    _diffuse = diff;
}



)";





/**************************** DefaultMVPLT_Opaque *****************************/


const char* DefaultMVPLT_Opaque_fs = R"(

#version 460
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform vec3 color;
uniform sampler2D tex;
uniform sampler2D shadow_map;

uniform vec3 light_pos;
uniform vec3 view_pos;

uniform float opacity = 1.0;
uniform bool use_transparent = false;

uniform mat4 tex_matrix;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(shadow_map, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light_pos - fs_in.FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
    //float bias = 0.001f;
    // check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x_ = -1; x_ <= 1; ++x_)
    {
        for(int y_ = -1; y_ <= 1; ++y_)
        {
            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x_, y_) * texelSize).r; 
//            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        // hard shadow
            shadow += currentDepth - bias > pcfDepth  ? pcfDepth : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    // adjust texture coordination
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);

    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);
//    vec2 texcoord_flipy = vec2(fs_in.TexCoords.x, 1.0 - fs_in.TexCoords.y);
    


    vec4 tex_color = texture(tex, texcoord_flipy);
        if(tex_color.a < 0.1) 
        discard;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.5);
    // ambient
    vec3 ambient = 0.7 * lightColor;
    // diffuse
    vec3 lightDir = normalize(light_pos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(view_pos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * tex_color.xyz * color;    
    //vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * tex_color.xyz * color;    
    

	FragColor = vec4(lighting, 1.0);
    if(use_transparent)
    {
		FragColor = vec4(tex_color.xyz* color, opacity);
    }
}

)";



const char* DefaultMVPLT_Opaque_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//uniform vec3 light_dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_matrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
//    vs_out.Normal = mat3(model) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = light_space_matrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);

}



)";





/****************************** DefaultParticle *******************************/


const char* DefaultParticle_fs = R"(

#version 460

uniform vec3 color;

out vec4 FragColor;

void main()
{   
    vec2 temp = gl_PointCoord - vec2(0.5);
    float f = dot(temp, temp);
    if (f>0.25) discard;
    FragColor = vec4(vec3(color), 1.0f);
}  

)";



const char* DefaultParticle_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 

)";





/***************************** DeferredComposite ******************************/


const char* DeferredComposite_fs = R"(

#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D position_map; // position + metallic
uniform sampler2D normal_map;   // normal + roughness
uniform sampler2D albedo_map;   // albedo + ambient
uniform sampler2D fluid_position_map; // position + metallic
uniform sampler2D fluid_normal_map;   // normal + roughness
uniform sampler2D fluid_albedo_map;   // albedo + ambient
uniform sampler2D custom_depth_map;   // r : opaque depth, g : fluid depth
uniform usampler2D custom_index_map;     // mask + nothing
uniform sampler2D depth_map;    // depth map
uniform sampler2D shadow_map;
uniform sampler2D ssao_map;
uniform sampler2D ssr_map;
uniform samplerCube skybox;

uniform vec3 background_color;
uniform vec3 view_pos;
uniform vec3 light_pos;
uniform mat4 light_space_matrix;

uniform bool b_skybox_reflection;
uniform bool b_ssr_reflection;

uniform bool use_fluid_map = false;


const float PI = 3.14159265359;
const uint CUSTOM_MASK_FLUID = 200;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 normal, vec3 halfway_dir, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(normal, halfway_dir), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 normal, vec3 view_dir, vec3 light_dir, float roughness)
{
    float NdotV = max(dot(normal, view_dir), 0.0);
    float NdotL = max(dot(normal, light_dir), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
float ShadowCalculation(vec4 FragPosLightSpace)
{
    // perform perspective divide
    vec3 proj_coords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(shadow_map, proj_coords.xy).r; 

    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    // calculate bias (based on depth map resolution and slope)

//    vec3 normal = normalize(fs_in.Normal);
    vec3 tangent_normal = texture(normal_map, TexCoords).rgb;
    vec3 normal = normalize(tangent_normal * 2.0 - 1.0);

    vec3 FragPos = texture(position_map, TexCoords).rgb;

    vec3 light_dir = normalize(light_pos - FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, light_dir)), 0.001);
    //float bias = 0.001f;
    // check whether current frag pos is in shadow
    //float shadow = current_depth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x_ = -1; x_ <= 1; ++x_)
    {
        for(int y_ = -1; y_ <= 1; ++y_)
        {
            float pcfDepth = texture(shadow_map, proj_coords.xy + vec2(x_, y_) * texelSize).r; 
//            shadow += current_depth - bias > pcfDepth  ? 1.0 : 0.0;        // hard shadow
            shadow += current_depth - bias > pcfDepth  ? pcfDepth : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
// ----------------------------------------------------------------------------

void main() {
    
    vec3 FragPos = texture(position_map, TexCoords).rgb;
    vec3 Normal = texture(normal_map, TexCoords).rgb;
    vec3 Albedo = texture(albedo_map, TexCoords).rgb;
    float metallic = texture(position_map, TexCoords).a;
    float roughness = texture(normal_map, TexCoords).a;
    float ambient = texture(albedo_map, TexCoords).a;
    float depth = texture(depth_map, TexCoords).r;
    float ssao = texture(ssao_map, TexCoords).r;
    vec4 FragPosLightSpace = light_space_matrix * vec4(FragPos, 1.0);
    uint custom_index = texture(custom_index_map, TexCoords).r; // mask + nothing
    vec4 custom_depth = texture(custom_depth_map, TexCoords); // r : opaque depth, g : fluid depth

    if(use_fluid_map && custom_index.r == CUSTOM_MASK_FLUID)
    {
        FragPos = texture(fluid_position_map, TexCoords).rgb;
        Normal = texture(fluid_normal_map, TexCoords).rgb;
        Albedo = texture(fluid_albedo_map, TexCoords).rgb;
        metallic = texture(fluid_position_map, TexCoords).a;
        roughness = texture(fluid_normal_map, TexCoords).a;
        ambient = texture(fluid_albedo_map, TexCoords).a;
    }

    

    if(depth >= 1.0)
    {
      FragColor = vec4(background_color, 1.0);
      return;
    }

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, metallic);


    vec3 lightDir = normalize(light_pos - FragPos);
    float diff = max(dot(lightDir, Normal), 0.0);
    vec3 viewDir = normalize(view_pos - FragPos);
    vec3 halfwayDir = normalize(viewDir + lightDir);

    float NDF = DistributionGGX(Normal, halfwayDir, roughness);
    float G = GeometrySmith(Normal, viewDir, lightDir, roughness);
    vec3 F = fresnelSchlick(clamp(dot(halfwayDir, viewDir), 0.0, 1.0), F0);

    vec3 numerator = NDF * G * F;
//    float denominator = 4 * max(dot(Normal, viewDir), 0.0) * max(dot(Normal, lightDir), 0.0);
    float denominator = 4 * max(dot(Normal, viewDir), 0.0) * max(dot(Normal, lightDir), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // kS = fresnel
    vec3 kS = F;
    // kD = 1 - kS
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;


    // shadow
    float shadow = ShadowCalculation(FragPosLightSpace);

    // scale light by NdotL
    float NdotL = max(dot(Normal, lightDir), 0.0);

//    vec3 lighting = (kD * Albedo / PI + specular) * diff * NdotL;
    vec3 lighting = (kD * Albedo / PI + specular) * NdotL;
    
    vec3 specularIBL = vec3(0.0);

    vec3 base = ambient * Albedo * ssao;

    bool reflection_passed = false;
    if(b_ssr_reflection)
    {
        // ssr reflection
        float ssr_alpha = texture(ssr_map, TexCoords).a;
        vec3 prefilterColor = texture(ssr_map, TexCoords).rgb;
        specularIBL += prefilterColor * F * ssr_alpha;

        // If specularIBL is not zero, then reflection is passed
        if(specularIBL != vec3(0.0))
            reflection_passed = true;
    }

    // skybox reflection
    if(b_skybox_reflection && !reflection_passed)
    {
        vec3 I = normalize(FragPos - view_pos);
        vec3 R = reflect(I, normalize(Normal));
        vec3 prefilterColor = textureLod(skybox, R, roughness * 5.0).rgb;
        specularIBL += prefilterColor * F;
        reflection_passed = true;
    }
    vec3 final_color = base + min((1.0 - shadow) * lighting + specularIBL, 1.0 - base);
//    if(reflection_passed)
//        final_color = mix(final_color, specularIBL, 0.75);

    // HDR tonemapping
    final_color = final_color / (final_color + vec3(1.0));
    // gamma correction
    float gamma = 2.2;
    final_color = pow(final_color, vec3(1.0/gamma));

    FragColor = vec4(final_color, 1.0);
}

)";





/******************************** DeferredQuad ********************************/


const char* DeferredQuad_vs = R"(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

)";





/*********************** DeferredScreenSpaceReflection ************************/


const char* DeferredScreenSpaceReflection_fs = R"(

#version 430 core
out vec4 final_reflection;
in vec2 TexCoords;
uniform sampler2D position_map;  // position + metallic
uniform sampler2D normal_map;    // normal(TBN)
uniform sampler2D albedo_map;    // albedo + ambient
uniform sampler2D fluid_position_map; // position + metallic
uniform sampler2D fluid_normal_map;   // normal + roughness
uniform sampler2D fluid_albedo_map;   // albedo + ambient
uniform sampler2D custom_depth_map;   // r : opaque depth, g : fluid depth
uniform usampler2D custom_mask_map;   // CUSTOM_MASK_FLUID = 200
uniform sampler2D depth_map;     // depth map

// Camera uniforms
uniform mat4 view;
uniform mat4 proj;
uniform vec3 cam_pos;
// SSR parameters
uniform float max_distance = 10.0;     // Maximum ray distance
uniform int max_steps = 100;           // Maximum ray steps 
uniform float step_size = 0.2;
uniform int binary_search_steps = 10;   // Binary search refinement steps
uniform float reflection_strength = 1.0;
uniform float near = 0.1;             // Near plane distance
uniform float far = 1000.0;           // Far plane distance

uniform float g_max_thickness = 0.1;
uniform float g_depthbias = 0.01;

uniform bool use_fluid_map = false;
const uint CUSTOM_MASK_FLUID = 200;
// ----------------------------------------------------------------------------
//vec2 WorldToScreen(vec3 world_pos)
//{
//    // Convert to clip space
//    vec4 clip_space_pos = proj * view * vec4(world_pos, 1.0);
//    // Perspective division to get NDC coordinates
//    vec2 ndc = clip_space_pos.xy / clip_space_pos.w;
//    // Convert from [-1,1] to [0,1] range
//    return ndc * 0.5 + 0.5;
//}
// ----------------------------------------------------------------------------
vec2 ViewToScreen(vec3 view_pos)
{
    // Convert to clip space
    vec4 clip_space_pos = proj * vec4(view_pos, 1.0);
    // Perspective division to get NDC coordinates
    vec2 ndc = clip_space_pos.xy / clip_space_pos.w;
    // Convert from [-1,1] to [0,1] range
    return ndc * 0.5 + 0.5;
}
// ----------------------------------------------------------------------------
//float GetLinearDepth(vec3 world_pos)
//{
//    vec4 clip_space_pos = proj * view * vec4(world_pos, 1.0);
//    float ndc_depth = clip_space_pos.z / clip_space_pos.w;
//    // Convert NDC depth to linear depth
//    return (2.0 * near * far) / (far + near - ndc_depth * (far - near));
//}
// ----------------------------------------------------------------------------
//float LinearizeDepth(float depth)
//{
//    // Convert normalized depth from [0,1] to NDC [-1,1]
//    float z = depth * 2.0 - 1.0;
//    // Convert NDC depth to linear depth
//    return (2.0 * near * far) / (far + near - z * (far - near));
//}
//float linearizeDepth (float zsample) {
//	return -far * near / (zsample * (far - near) - far);
//}

//float CalculateViewDepthFromDepthMap(float depth)
//{
//    /*
//        float viewZ = nearPlane + normalizedDepth * (farPlane - nearPlane);
//        return -viewZ;
//    */
//    float viewZ = near + depth * (far - near);
//    return -viewZ;
//}

// ----------------------------------------------------------------------------
bool IsOutsideScreen(vec2 tex_coords)
{
    return any(lessThan(tex_coords, vec2(0.0))) || any(greaterThan(tex_coords, vec2(1.0)));
}

bool IsOutsideScreenFromView(vec3 view_pos)
{
    // If view_pos'z is negative, it's behind the camera
    if (view_pos.z > 0.0) return true;

    // Convert to clip space
    vec4 clip_space_pos = proj * vec4(view_pos, 1.0);
    // Perspective division to get NDC coordinates
    vec2 ndc = clip_space_pos.xy / clip_space_pos.w;
    // Convert from [-1,1] to [0,1] range
    vec2 screen_coords = ndc * 0.5 + 0.5;
    // Check if outside screen
    return any(lessThan(screen_coords, vec2(0.0))) || any(greaterThan(screen_coords, vec2(1.0)));

}

float GetScreenEdgeFactor(vec2 screen_pos)
{
  /*
    vec2 dCoords = abs(vec2(0.5, 0.5) - coords.xy);
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
  */
  vec2 dCoords = abs(vec2(0.5, 0.5) - screen_pos);
  float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
  return screenEdgefactor;
}

//vec3 GetViewPosFromDepthMap(vec2 tex_coords)
//{
//    // Get depth value from depth map
//    float depth = texture(depth_map, tex_coords).r;
//    // Convert to view space position
//    vec4 clip_space_pos = vec4(tex_coords * 2.0 - 1.0, depth, 1.0);
//    vec4 view_space_pos = inverse(proj) * clip_space_pos;
//    view_space_pos /= view_space_pos.w;
//    return view_space_pos.xyz;
//}
// ----------------------------------------------------------------------------
void main() {

    // 0.0 - opaque, 1.0 - fluid
    float opaque_fluid_balancing = (use_fluid_map && texture(custom_mask_map, TexCoords).r == CUSTOM_MASK_FLUID) ? 1.0 : 0.0; 
    // Get normal, position, albedo and depth from the maps

    vec3 normal = mix(texture(normal_map, TexCoords).rgb, texture(fluid_normal_map, TexCoords).rgb, opaque_fluid_balancing);
    vec3 frag_pos = mix(texture(position_map, TexCoords).rgb, texture(fluid_position_map, TexCoords).rgb, opaque_fluid_balancing);
    vec3 albedo = mix(texture(albedo_map, TexCoords).rgb, texture(fluid_albedo_map, TexCoords).rgb, opaque_fluid_balancing);
    float depth = mix(texture(depth_map, TexCoords).r, texture(custom_depth_map, TexCoords).g, opaque_fluid_balancing);
//    vec3 normal = normalize(texture(normal_map, TexCoords).rgb);
//    vec3 frag_pos = texture(position_map, TexCoords).rgb;
//    vec3 albedo = texture(albedo_map, TexCoords).rgb;
//    float depth = texture(depth_map, TexCoords).r;
    
    // Check if surface is reflective
//    float metallic = texture(position_map, TexCoords).a;
    float metallic = mix(texture(normal_map, TexCoords).a, texture(fluid_normal_map, TexCoords).a, opaque_fluid_balancing);
    if (metallic < 0.01) {
        final_reflection = vec4(0.0);
        return;
    }
    
    // Convert to view pos from world pos
    vec4 view_pos = view * vec4(frag_pos, 1.0);
    view_pos /= view_pos.w;
    // Calculate view direction 
    vec3 view_dir = normalize(view_pos.xyz);

    vec4 view_normal = view * vec4(normal, 0.0);

    // Get reflection vector
    vec3 reflect_dir = normalize(reflect(view_dir, view_normal.xyz));
    
    
    float step_size = max_distance / float(max_steps);

    // The calculation run on view space.
    // The ray is casted from the fragment position in the direction of the reflection vector.

    // Initialize ray trace
    vec3 cur_pos = view_pos.xyz;
    bool hit = false;
    vec2 hit_pixel = vec2(0.0);
    
    vec2 screen_pos;
    // Ray marching for SSR
    for(int i = 0; i < max_steps; ++i)
    {
        // Get next position
        cur_pos += reflect_dir * step_size;
        
        // Get screen space coordinates
        screen_pos = ViewToScreen(cur_pos);
        
        // Check if outside screen
        if(IsOutsideScreenFromView(cur_pos))
            break;
            
        // Get sampled depth at current screen position and linearize it
        uint custom_screen_pos_index = texture(custom_mask_map, screen_pos).r; // mask + nothing
//        float cur_depth = LinearizeDepth(texture(depth_map, screen_pos).r); // view space depth
        float cur_depth = (custom_screen_pos_index == CUSTOM_MASK_FLUID) ? texture(custom_depth_map, screen_pos).g : texture(custom_depth_map, screen_pos).r;

        // Get depth of current ray position
        float ray_depth = -cur_pos.z;
        float diff = ray_depth - cur_depth;

        if(diff > g_depthbias  && diff < g_max_thickness)
        {
          hit = true;
          hit_pixel = ViewToScreen(cur_pos);

          // Binary search for more accurate hit position
          vec3 prev_pos = cur_pos - reflect_dir * step_size;
          vec3 mid_pos = cur_pos;
          for(int j = 0; j < binary_search_steps; ++j)
          {
              mid_pos = mix(prev_pos, cur_pos, 0.5);

              if(!IsOutsideScreenFromView(mid_pos))
              {
//                float mid_depth = LinearizeDepth(texture(depth_map, ViewToScreen(mid_pos)).r);

                uint custom_view_to_screen_pos_index = texture(custom_mask_map, ViewToScreen(mid_pos)).r; // mask + nothing
                float mid_depth = (custom_view_to_screen_pos_index == CUSTOM_MASK_FLUID) ? texture(custom_depth_map, ViewToScreen(mid_pos)).g : texture(custom_depth_map, ViewToScreen(mid_pos)).r;
                float mid_ray_depth = -mid_pos.z;

                if(mid_ray_depth > mid_depth)
                    cur_pos = mid_pos;
                else
                    prev_pos = mid_pos;
              }
          }
         hit_pixel = ViewToScreen(mid_pos);
         break;
        }
    }

    // If hit, calculate final reflection
    if(hit && !IsOutsideScreen(hit_pixel))
    {
//      vec3 reflected_color = texture(albedo_map, hit_pixel).rgb;
      uint hit_mask = texture(custom_mask_map, hit_pixel).r; // mask + nothing
      vec3 reflected_color = (hit_mask == CUSTOM_MASK_FLUID) ? texture(fluid_albedo_map, hit_pixel).rgb : texture(albedo_map, hit_pixel).rgb;
      final_reflection = vec4(reflected_color, GetScreenEdgeFactor(hit_pixel) * reflection_strength * metallic);
      return;
    }

    final_reflection = vec4(0.0);
}

)";





/********************* DeferredScreenSpaceReflectionBlur **********************/


const char* DeferredScreenSpaceReflectionBlur_fs = R"(

#version 430 core
out vec4 final_reflection;

in vec2 TexCoords;

uniform sampler2D ssr_texture;
uniform sampler2D normal_map; // normal + roughness.
uniform sampler2D fluid_normal_map; // normal + roughness.
uniform usampler2D custom_mask_map; // CUSTOM_MASK_FLUID = 200

uniform float blur_coefficient = 1.0;
uniform bool use_fluid_map = false;
const uint CUSTOM_MASK_FLUID = 200;

void main() {
    // 0.0 - opaque, 1.0 - fluid
    float opaque_fluid_balancing = (use_fluid_map && texture(custom_mask_map, TexCoords).r == CUSTOM_MASK_FLUID) ? 1.0 : 0.0; 

	// This blur is used to Gaussian 3x3 blur the reflection.
	// The blur kernel function based on the url https://en.wikipedia.org/wiki/Kernel_(image_processing)

	// As roughness, it will be used to blur the reflection.

	// 0 - no blur. 1 - full blur.
	vec2 pixel_size = 1.0 / textureSize(ssr_texture, 0);

//	float roughness = texture(normal_map, TexCoords).a;
	float roughness = mix(texture(normal_map, TexCoords).a, texture(fluid_normal_map, TexCoords).a, opaque_fluid_balancing); 

	// blur weight array 3x3. 
	const vec2 blur_weights[9] = vec2[](
	vec2(-1.0, -1.0),	vec2(0.0, -1.0),	vec2(1.0, -1.0),
	vec2(-1.0, 0.0),	vec2(0.0, 0.0),		vec2(1.0, 0.0),
	vec2(-1.0, 1.0),	vec2(0.0, 1.0),		vec2(1.0, 1.0)
	);

	const float gaussian_blur_weight[9] = float[](
		1, 2, 1,
		2, 4, 2,
		1, 2, 1
	);

	// Blur the reflection.
	vec4 reflection = texture(ssr_texture, TexCoords);
	vec3 blurred_reflection = vec3(0.0);
	vec2 blur_base = pixel_size * vec2(2.0);
	float gaussian_blur_weight_point_sum = 0.0f;

	for (int i = 0; i < 9; i++) {
		vec2 offset = blur_base + gaussian_blur_weight[i] * blur_coefficient * pixel_size * blur_weights[i] * roughness;
		vec3 sampled_color = texture(ssr_texture, TexCoords + offset).rgb;
		blurred_reflection += sampled_color;
		gaussian_blur_weight_point_sum += gaussian_blur_weight[i];
	}
	blurred_reflection /= gaussian_blur_weight_point_sum;
	final_reflection = vec4(blurred_reflection, reflection.a);
}


)";





/******************************** DeferredSSAO ********************************/


const char* DeferredSSAO_fs = R"(

#version 430 core
out float final_occlusion;

in vec2 TexCoords;

uniform sampler2D position_map;  // position + metallic
uniform sampler2D normal_map;    // normal(TBN)
uniform sampler2D fluid_position_map; // position + metallic
uniform sampler2D fluid_normal_map;   // normal + roughness
uniform usampler2D custom_mask_map;   // CUSTOM_MASK_FLUID = 200
uniform sampler2D noise_texture; // noise texture


uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernel_size = 64;
float radius = 0.5;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec2 screen_size;
const float noise_size = 4.0;
//const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 

uniform mat4 view;
uniform mat4 projection;

uniform bool use_fluid_map = false;
const uint CUSTOM_MASK_FLUID = 200;

void main() {
    // 0.0 - opaque, 1.0 - fluid
    float opaque_fluid_balancing = (use_fluid_map && texture(custom_mask_map, TexCoords).r == CUSTOM_MASK_FLUID) ? 1.0 : 0.0; 

    // get world-space position and normal from gbuffer
    vec3 world_position = mix(texture(position_map, TexCoords).rgb, texture(fluid_position_map, TexCoords).rgb, opaque_fluid_balancing); 
    // TBN Texture in space
    vec3 normal         = mix(texture(normal_map, TexCoords).rgb, texture(fluid_normal_map, TexCoords).rgb, opaque_fluid_balancing); 

    // get view-space position
    vec4 view_space_pos = view * vec4(world_position, 1.0);
    vec3 view_pos = view_space_pos.xyz / view_space_pos.w;

    // convert normal from TBN to view-space
    vec3 view_normal = normalize(mat3(view) * normal);

    vec2 noise_scale = screen_size / noise_size;

    // get random rotation vec3
    vec3 random_vec = texture(noise_texture, TexCoords * noise_scale).xyz * 2.0 - 1.0;

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;

    for(int i = 0; i < kernel_size; ++i)
    {
        // get sample position
        vec3 sample_pos = samples[i]; // from tangent to view-space

        vec2 rotated = vec2(
        random_vec.x * sample_pos.x - random_vec.y * sample_pos.y,
        random_vec.y * sample_pos.x + random_vec.x * sample_pos.y);

        sample_pos = vec3(rotated, sample_pos.z);
        sample_pos = view_pos + (sample_pos) * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = projection * vec4(sample_pos, 1.0);
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // sample position texture using projected coordinates
        uint sample_mask = texture(custom_mask_map, offset.xy).r; // mask + nothing
        vec3 sample_world_pos = sample_mask == CUSTOM_MASK_FLUID ? texture(fluid_position_map, offset.xy).rgb : texture(position_map, offset.xy).rgb;
        vec4 sample_view_pos = view * vec4(sample_world_pos, 1.0);
        sample_view_pos.xyz /= sample_view_pos.w;

        //float sample_depth = texture(position_map, offset.xy).z;

        // range check & accumulate
        float range_check = smoothstep(0.0, 1.0, radius / abs(view_pos.z - sample_view_pos.z));
        occlusion += (sample_view_pos.z >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;
    }
    occlusion = 1.0 - occlusion / kernel_size;

    final_occlusion = occlusion;
}

)";





/****************************** DeferredSSAOBlur ******************************/


const char* DeferredSSAOBlur_fs = R"(

#version 430 core
out float final_occlusion;

in vec2 TexCoords;

uniform sampler2D ssao_texture;

void main() {
    vec2 texel_size = 1.0 / vec2(textureSize(ssao_texture, 0));

    float result = 0.0;

    for(int x = -2; x <= 2; ++x) {
        for(int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(ssao_texture, TexCoords + offset).r;
        }
    }

    final_occlusion = result / 25.0;
}

)";





/*********************************** Depth ************************************/


const char* Depth_fs = R"(

#version 460
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 Position;
in vec2 TexCoords;
in vec3 Normal;

uniform vec3 color;
uniform sampler2D tex;

void main()
{
    vec2 texcoord_flipy = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec4 tex_color = texture(tex, texcoord_flipy);
            if(tex_color.a < 0.1) 
            discard;
    
    gPosition = Position;
    gNormal = normalize(Normal);
    gAlbedoSpec.xyz = color * tex_color.rgb;
    gAlbedoSpec.a = tex_color.a;
}

    #version 330 core

    uniform float near;
    uniform float far;

    float LinearizeDepth(float depth) 
    {
        float z = depth * 2.0 - 1.0; // back to NDC 
        return (2.0 * near * far) / (far + near - z * (far - near));	
    }


)";



const char* Depth_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Position;
out vec2 TexCoords;
out vec3 Normal;


void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    
}


)";





/******************************** DMaterialPbr ********************************/


const char* DMaterialPbr_fs = R"(

#version 460
layout (location = 0) out vec4 gPosition; // position + metallic
layout (location = 1) out vec4 gNormal;   // normal + roughness
layout (location = 2) out vec4 gAlbedoSpec; // albedo + dummy(nothing)
layout (location = 6) out vec4 gCustomDepth; // r : opaque, g : fluid. linear depth
layout (location = 7) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;

uniform vec3 color;

uniform float metallic;
uniform float roughness;
uniform float ambient = 0.3;

uniform mat4 tex_matrix;
uniform float near;
uniform float far;

const uint CUSTOM_MASK_OPAQUE = 100;

// ----------------------------------------------------------------------------
vec3 GetNormalFromMap()
{
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);
    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);

    vec3 tangent_normal = texture(normal_map, texcoord_flipy).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    float det = st1.s * st2.t - st2.s * st1.t;
    float epsilon = 0.000001;
    if(abs(det) < epsilon) {
        return normalize(fs_in.Normal);
    }

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
//    T = normalize(T - dot(T, N) * N);
    vec3 B  = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangent_normal);
}

// ----------------------------------------------------------------------------
float LinearDepth(float depth)
{
    // Calculate the linear depth from the view matrix and projection matrix
    float z = depth * 2.0 - 1.0; // Convert to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}
// ----------------------------------------------------------------------------

void main()
{
    // adjust texture coordination
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);

    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);
    
    vec4 tex_color = texture(diffuse_map, texcoord_flipy);
        if(tex_color.a < 0.1) 
        discard;

    gPosition.xyz = fs_in.FragPos;
    gPosition.w = metallic;

    gNormal.xyz = GetNormalFromMap();
    gNormal.w = roughness;

    gAlbedoSpec.xyz = tex_color.rgb * color;
    gAlbedoSpec.w = ambient;

    gCustomIndex = uvec4(CUSTOM_MASK_OPAQUE); // nothing
    gCustomDepth.r = LinearDepth(gl_FragCoord.z); // linear depth
    gCustomDepth.a = 1.0;

//    gCustomIndex.r = CUSTOM_MASK_OPAQUE; // Opaque mask
}

)";



const char* DMaterialPbr_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//uniform vec3 light_dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

void main()
{
    vec4 world_pos = model * vec4(aPos, 1.0);
    vs_out.FragPos = world_pos.xyz;
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;

    gl_Position = projection * view * world_pos ;

}



)";





/****************************** DParticleRender *******************************/


const char* DParticleRender_fs = R"(

#version 460
layout (location = 3) out vec4 gPosition; // position + metallic
layout (location = 4) out vec4 gNormal;   // normal + roughness
layout (location = 5) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 6) out vec4 gCustomDepth; // r : opaque, g : fluid. linear depth
layout (location = 7) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec4 point_min_point_type = vec4(0, 0, 0, 0);
uniform vec4 vector_max_radius_inout = vec4(0, 0, 0, 1);

uniform bool use_legend = false;
uniform bool use_texture_second = false;

// Should get min/max value
uniform float max_value = 10.0;
uniform float min_value = 0.0;

uniform mat4 view;
uniform mat4 projection;
uniform float near;
uniform float far;

uniform float ambient = 0.8;

// Water INFO
// metallic = 0.0
// roughness = 0.01
// IOR = 1.333
// Specular = 0.25

//const float water_metallic = 0.0;
const float water_metallic = 0.8; // for debugging
//const float water_roughness = 0.01;
const float water_roughness = 0.2; // for debugging 
const float water_IOR = 1.333;
const float water_specular = 0.25;


const uint CUSTOM_MASK_FLUID = 200;

float LinearDepth(float depth)
{
    // Calculate the linear depth from the view matrix and projection matrix
    float z = depth * 2.0 - 1.0; // Convert to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}
// ----------------------------------------------------------------------------
int clip_plane(vec3 point, vec3 normal, vec3 world_pos) {
	return (dot(world_pos - point, normal) < 0) ? 1 : -1;
}
// ----------------------------------------------------------------------------
int clip_box(vec3 min, vec3 max, vec3 world_pos) {
	return (world_pos.x < min.x || world_pos.x > max.x || world_pos.y < min.y || world_pos.y > max.y || world_pos.z < min.z || world_pos.z > max.z) ? -1 : 1;
}
// ----------------------------------------------------------------------------
int clip_sphere(vec3 center, float radius, vec3 world_pos) {
	return (length(world_pos - center) < radius) ? 1 : -1;
}
// ----------------------------------------------------------------------------
void main()
{
 // calculate clipping
 switch (int(point_min_point_type.w)) {
    // plane clipping case
	case 1:
        // calculate that world position is above the plane which is defined with 3d point and 3d vector.
        // position = point_min_point_type.xyz
        // vector = vector_max_radius_inout.xyz
        if(clip_plane(point_min_point_type.xyz, vector_max_radius_inout.xyz, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;

        // cube min max clipping case
	case 2:
        // calculate that world position is inside the box which is defined with min point and max point.
        // min = point_min_point_type.xyz
        // max = vector_max_radius_inout.xyz
        if(clip_box(point_min_point_type.xyz, vector_max_radius_inout.xyz, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;

        // sphere clipping case
	case 3:
        // calculate that world position is inside the sphere which is defined with center point and radius.
        // center = point_min_point_type.xyz
        // radius = vector_max_radius_inout.x
        if(clip_sphere(point_min_point_type.xyz, vector_max_radius_inout.x, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;
    default : 
        break;
    }
    // Calculate normal from point

    // if out of circle, discard
    vec2 pointUV = gl_PointCoord;

    vec3 normal;
    vec2 normal_point_uv = pointUV;
    normal_point_uv.y = 1.0 - normal_point_uv.y;
    normal.xy = normal_point_uv * 2.0 - 1.0;
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard;
    normal.z = (sqrt(1.0 - r2)) * 2.0 - 1.0;

    vec4 world_normal = inverse(view) * vec4(normal, 0.0);
    vec3 view_normal_dir = normalize(world_normal.xyz);
    gNormal = vec4(view_normal_dir, water_roughness);
    gPosition = vec4(fs_in.world_pos, water_metallic);

    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
    gCustomDepth.g = LinearDepth(gl_FragCoord.z); // linear depth near to far
    gCustomDepth.a = 1.0;
    
//    gCustomIndex.r = CUSTOM_MASK_FLUID; // Opaque mask


    // calculate legend texture color
    if(use_legend) {

        float val = length(fs_in.val_color);
        // calculate val to 0~1 (min, max)
        float val_per = clamp(1.0 - ((val - min_value) / (max_value - min_value)), 0.005, 0.995);
        // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
        vec2 legend_color = vec2(val_per, val_per);
        //vec4 tex0_color = texture(tex0, legend_color);
        vec4 tex0_color = texture(tex0, legend_color);

        vec4 tex1_color = vec4(1.0, 1.0, 1.0, ambient);

        if(use_texture_second)
        {
            tex1_color = texture(tex1, pointUV);

            if(tex1_color.a < 0.1) discard;
        }
        gAlbedoSpec = tex0_color * tex1_color;
       return;
    }
    else{
    //  val_color is -10 ~ 10. convert to 0~1
    vec3 satured_color = fs_in.val_color * 0.05 + 0.5;
    //gAlbedoSpec = vec4(satured_color, 1.0f);
    gAlbedoSpec = vec4(0.824, 0.91, 0.93, ambient);
    }

}

)";



const char* DParticleRender_vs = R"(

#version 460
layout(std430, binding = 0) buffer PositionOut {
    vec4 positions[];
};

layout(std430, binding = 1) buffer VelocityOut {
    vec4 velocities[];
};

layout(std430, binding = 2) buffer AccelerationOut {
	vec4 accelerations[];
};

layout(location = 0) in uint instanceID;


out VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_position;
// 0 - none
// 1 - velocity
// 2 - acceleration
// 3 - density (position.w)
// 4 - pressure (velocity.w)
uniform int color_type = 0;

float screen_size = 1024.0f;

void main()
{
	if(accelerations[instanceID].w == -1.0)
	{
		gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		return;
	}

    vs_out.val_color = vec3(0.0);
    switch(color_type) {
		case 0:
			vs_out.val_color = vec3(velocities[instanceID].xyz);
			break;
		case 1:
			vs_out.val_color = vec3(velocities[instanceID].xyz);
			break;
		case 2:
			vs_out.val_color = vec3(accelerations[instanceID].xyz);
			break;
		case 3:
			vs_out.val_color = vec3(positions[instanceID].w, 0, 0);
			break;
		case 4:
			vs_out.val_color = vec3(velocities[instanceID].w, 0, 0);
			break;
		default:
			vs_out.val_color = vec3(1.0);
			break;
	}




    vec3 pos = positions[instanceID].xyz;
    gl_Position = projection * view * vec4(pos, 1.0);
    vs_out.world_pos = pos;

    float distance_from_camera = max(length(camera_position - pos), 0.001);

    //gl_Position = vec4(pos, 1.0);
    gl_PointSize = screen_size / distance_from_camera;

}



)";





/******************************* DPointClipper ********************************/


const char* DPointClipper_fs = R"(

#version 460
layout (location = 0) out vec4 gPosition; // position + metallic
layout (location = 1) out vec4 gNormal;   // normal + roughness
layout (location = 2) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 7) out uvec4 gCustomIndex; // mask, nothing 3
//layout (location = 3) out vec4 gDepth;

in VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec4 point_min_point_type = vec4(0, 0, 0, 0);
uniform vec4 vector_max_radius_inout = vec4(0, 0, 0, 1);

uniform bool use_legend = false;
uniform bool use_texture_second = false;

// Should get min/max value
uniform float max_value = 10.0;
uniform float min_value = 0.0;

uniform mat4 view;
uniform mat4 projection;

uniform float ambient = 0.8;

// Water INFO
// metallic = 0.0
// roughness = 0.01
// IOR = 1.333
// Specular = 0.25

//const float water_metallic = 0.0;
const float water_metallic = 0.8; // for debugging
//const float water_roughness = 0.01;
const float water_roughness = 0.2; // for debugging 
const float water_IOR = 1.333;
const float water_specular = 0.25;

const uint CUSTOM_MASK_FLUID = 200;


// ----------------------------------------------------------------------------
int clip_plane(vec3 point, vec3 normal, vec3 world_pos) {
	return (dot(world_pos - point, normal) < 0) ? 1 : -1;
}
// ----------------------------------------------------------------------------
int clip_box(vec3 min, vec3 max, vec3 world_pos) {
	return (world_pos.x < min.x || world_pos.x > max.x || world_pos.y < min.y || world_pos.y > max.y || world_pos.z < min.z || world_pos.z > max.z) ? -1 : 1;
}
// ----------------------------------------------------------------------------
int clip_sphere(vec3 center, float radius, vec3 world_pos) {
	return (length(world_pos - center) < radius) ? 1 : -1;
}
// ----------------------------------------------------------------------------
void main()
{
 // calculate clipping
 switch (int(point_min_point_type.w)) {
    // plane clipping case
	case 1:
        // calculate that world position is above the plane which is defined with 3d point and 3d vector.
        // position = point_min_point_type.xyz
        // vector = vector_max_radius_inout.xyz
        if(clip_plane(point_min_point_type.xyz, vector_max_radius_inout.xyz, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;

        // cube min max clipping case
	case 2:
        // calculate that world position is inside the box which is defined with min point and max point.
        // min = point_min_point_type.xyz
        // max = vector_max_radius_inout.xyz
        if(clip_box(point_min_point_type.xyz, vector_max_radius_inout.xyz, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;

        // sphere clipping case
	case 3:
        // calculate that world position is inside the sphere which is defined with center point and radius.
        // center = point_min_point_type.xyz
        // radius = vector_max_radius_inout.x
        if(clip_sphere(point_min_point_type.xyz, vector_max_radius_inout.x, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;
    default : 
        break;
    }
    // Calculate normal from point

    // if out of circle, discard
    vec2 pointUV = gl_PointCoord;

    vec3 normal;
    vec2 normal_point_uv = pointUV;
    normal_point_uv.y = 1.0 - normal_point_uv.y;
    normal.xy = normal_point_uv * 2.0 - 1.0;
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard;
    normal.z = (sqrt(1.0 - r2)) * 2.0 - 1.0;

    vec4 world_normal = inverse(view) * vec4(normal, 0.0);
    vec3 view_normal_dir = normalize(world_normal.xyz);
    gNormal = vec4(view_normal_dir, water_roughness);
    gPosition = vec4(fs_in.world_pos, water_metallic);

    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
//    gCustomIndex.r = CUSTOM_MASK_FLUID; // Opaque mask


    // calculate legend texture color
    if(use_legend) {

        float val = length(fs_in.val_color);
        // calculate val to 0~1 (min, max)
        float val_per = clamp(1.0 - ((val - min_value) / (max_value - min_value)), 0.005, 0.995);
        // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
        vec2 legend_color = vec2(val_per, val_per);
        //vec4 tex0_color = texture(tex0, legend_color);
        vec4 tex0_color = texture(tex0, legend_color);

        vec4 tex1_color = vec4(1.0, 1.0, 1.0, ambient);

        if(use_texture_second)
        {
            tex1_color = texture(tex1, pointUV);

            if(tex1_color.a < 0.1) discard;
        }
        gAlbedoSpec = tex0_color * tex1_color;
       return;
    }
    else{
    //  val_color is -10 ~ 10. convert to 0~1
    vec3 satured_color = fs_in.val_color * 0.05 + 0.5;
    //gAlbedoSpec = vec4(satured_color, 1.0f);
    gAlbedoSpec = vec4(0.824, 0.91, 0.93, ambient);

    }

}

)";



const char* DPointClipper_vs = R"(

#version 460
layout(std430, binding = 0) buffer PositionOut {
    vec4 positions[];
};

layout(std430, binding = 1) buffer VelocityOut {
    vec4 velocities[];
};

layout(std430, binding = 2) buffer AccelerationOut {
	vec4 accelerations[];
};

layout(location = 0) in uint instanceID;


out VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_position;
// 0 - none
// 1 - velocity
// 2 - acceleration
// 3 - density (position.w)
// 4 - pressure (velocity.w)
uniform int color_type = 0;

float screen_size = 1024.0f;

void main()
{
	if(accelerations[instanceID].w == -1.0)
	{
		gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		return;
	}

    vs_out.val_color = vec3(0.0);
    switch(color_type) {
		case 0:
			vs_out.val_color = vec3(velocities[instanceID].xyz);
			break;
		case 1:
			vs_out.val_color = vec3(velocities[instanceID].xyz);
			break;
		case 2:
			vs_out.val_color = vec3(accelerations[instanceID].xyz);
			break;
		case 3:
			vs_out.val_color = vec3(positions[instanceID].w, 0, 0);
			break;
		case 4:
			vs_out.val_color = vec3(velocities[instanceID].w, 0, 0);
			break;
		default:
			vs_out.val_color = vec3(1.0);
			break;
	}




    vec3 pos = positions[instanceID].xyz;
    gl_Position = projection * view * vec4(pos, 1.0);
    vs_out.world_pos = pos;

    float distance_from_camera = max(length(camera_position - pos), 0.001);

    //gl_Position = vec4(pos, 1.0);
    gl_PointSize = screen_size / distance_from_camera;

}



)";





/**************************** DPointMultiFloatVtk *****************************/


const char* DPointMultiFloatVtk_fs = R"(

#version 460
layout (location = 3) out vec4 gPosition; // position + metallic
layout (location = 4) out vec4 gNormal;   // normal + roughness
layout (location = 5) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 6) out vec4 gCustomDepth; // r : opaque, g : fluid. linear depth
layout (location = 7) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform vec3 value_min;
uniform vec3 value_max;

uniform mat4 view;
uniform float near;
uniform float far;
uniform float ambient = 0.8;

//const float water_metallic = 0.0;
const float water_metallic = 0.8; // for debugging
//const float water_roughness = 0.01;
const float water_roughness = 0.2; // for debugging 
const float water_IOR = 1.333;
const float water_specular = 0.25;
const uint CUSTOM_MASK_FLUID = 200;

float LinearDepth(float depth)
{
    // Calculate the linear depth from the view matrix and projection matrix
    float z = depth * 2.0 - 1.0; // Convert to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{   
    vec2 pointUV = gl_PointCoord;
    
    // Calculate normal vector from point coordinates
    vec2 normalPointUV = vec2(pointUV.x, 1.0 - pointUV.y);
    vec3 normal;
    normal.xy = normalPointUV * 2.0 - 1.0;

    // Check if point is within unit circle
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard; // Discard fragments outside the circle

    vec4 tex0_color = texture(tex0, pointUV);

    // Calculate z component using hemisphere formula
    normal.z = sqrt(1.0 - r2) * 2.0 - 1.0;

    // Transform from view space to world space
    vec4 worldNormal = inverse(view) * vec4(normal, 0.0);
    vec3 viewNormalDir = normalize(worldNormal.xyz);

    // Output normal and roughness value
    gNormal = vec4(viewNormalDir, water_roughness);

    vec3 value_color = fs_in.val_color;
    // value_color should changed 0~1 by value_min~value_max for xyz
    vec3 min_max_diff = value_max - value_min;

    // When any element of diff is 0, then the element should be 0.1.
    min_max_diff = max(min_max_diff, vec3(0.1, 0.1, 0.1));

    value_color = (value_color - value_min) / min_max_diff;

//    // if any the value is out of the value_min and value_max, then the color should be black.
//    if(value_color.r < 0.0 || value_color.r > 1.0 || value_color.g < 0.0 || value_color.g > 1.0 || value_color.b < 0.0 || value_color.b > 1.0)
//    {
//        value_color = vec4(0.2, 0.2, 0.2, 0.3);
//    }


    gPosition = vec4(fs_in.world_pos, water_metallic);
    gAlbedoSpec = vec4(value_color * tex0_color.rgb, ambient);
    
    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
    gCustomDepth.g = LinearDepth(gl_FragCoord.z); // linear depth near to far
    gCustomDepth.a = 1.0;

}  

)";



const char* DPointMultiFloatVtk_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor; 

uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

uniform bool b_clip_invert;
uniform bool b_legend_filtering;
uniform int clipping_type; // 0 - NONE, 1 - CUBE, 2 - SPHERE, 3 - PLANE
uniform mat4 clipping_obj_transform;

uniform vec3 value_min;
uniform vec3 value_max;

out VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} vs_out;

void main()
{
// cube
    if(clipping_type == 1 && b_clip_invert) // return outside of the cube
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 1 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if ((abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // sphere
    else if(clipping_type == 2 && b_clip_invert) // return outside of the sphere
    {
        // Get calculted sphere's radius from x-scaling.
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) < 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 2 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) > 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // plane. If the point is on the positive side of the plane, return it.
    else if(clipping_type == 3 && b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        // the origin plane's normal vector is (0, 1, 0). If the dot product of the point and the normal vector is positive, the point is on the positive side of the plane.
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) > 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 3 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) < 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // If the point is outside of the legend range, the particle is not drawn.
    if(b_legend_filtering && (aColor.x < value_min.x || aColor.x > value_max.x || aColor.y < value_min.y || aColor.y > value_max.y || aColor.z < value_min.z || aColor.z > value_max.z))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }

    vs_out.val_color = aColor;
    vs_out.world_pos = aPos;

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 

)";





/**************************** DPointSingleFloatVtk ****************************/


const char* DPointSingleFloatVtk_fs = R"(

#version 460
layout (location = 3) out vec4 gPosition; // position + metallic
layout (location = 4) out vec4 gNormal;   // normal + roughness
layout (location = 5) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 6) out vec4 gCustomDepth; // r : opaque, g : fluid. linear depth
layout (location = 7) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    float val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform float legend_min = 0;
uniform float legend_max = 10;

uniform bool use_legend = false;

uniform mat4 view;
uniform float near;
uniform float far;
uniform float ambient = 0.8;

//const float water_metallic = 0.0;
const float water_metallic = 0.8; // for debugging
//const float water_roughness = 0.01;
const float water_roughness = 0.2; // for debugging 
const float water_IOR = 1.333;
const float water_specular = 0.25;
const uint CUSTOM_MASK_FLUID = 200;

float LinearDepth(float depth)
{
    // Calculate the linear depth from the view matrix and projection matrix
    float z = depth * 2.0 - 1.0; // Convert to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{   
    vec2 pointUV = gl_PointCoord;
    
    // Calculate normal vector from point coordinates
    vec2 normalPointUV = vec2(pointUV.x, 1.0 - pointUV.y);
    vec3 normal;
    normal.xy = normalPointUV * 2.0 - 1.0;

    // Check if point is within unit circle
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard; // Discard fragments outside the circle

    vec4 tex0_color = texture(tex0, pointUV);

    // Calculate z component using hemisphere formula
    normal.z = sqrt(1.0 - r2) * 2.0 - 1.0;

    // Transform from view space to world space
    vec4 worldNormal = inverse(view) * vec4(normal, 0.0);
    vec3 viewNormalDir = normalize(worldNormal.xyz);

    // Output normal and roughness value
    gNormal = vec4(viewNormalDir, water_roughness);

    //tex1 : legend texture
    float val = fs_in.val_color;
    // calculate val to 0~1 (min, max)
    float val_per = clamp(1.0 - ( float(val - legend_min) / float(legend_max - legend_min)), 0.005, 0.995);
    // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
    vec2 legend_color = vec2(val_per, val_per);

//    // if the value is out of the legend range, then the color should be black.
//    if(val < legend_min || val > legend_max)
//    {
//        FragColor = vec4(0.2, 0.2, 0.2, 0.3);
//        return;
//    }

    vec4 tex1_color = texture(tex1, legend_color);
    
    gPosition = vec4(fs_in.world_pos, water_metallic);
    gAlbedoSpec = vec4(tex1_color.rgb * tex0_color.rgb, ambient);
    
    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
    gCustomDepth.g = LinearDepth(gl_FragCoord.z); // linear depth near to far
    gCustomDepth.a = 1.0;

}  

)";



const char* DPointSingleFloatVtk_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in float aValue; 

uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

uniform bool b_clip_invert;
uniform bool b_legend_filtering;
uniform int clipping_type; // 0 - NONE, 1 - CUBE, 2 - SPHERE, 3 - PLANE
uniform mat4 clipping_obj_transform;

uniform float legend_min = 0;
uniform float legend_max = 10;

out VS_OUT {
    float val_color;
    vec3 world_pos;
} vs_out;

void main()
{
// cube
    if(clipping_type == 1 && b_clip_invert) // return outside of the cube
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 1 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if ((abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // sphere
    else if(clipping_type == 2 && b_clip_invert) // return outside of the sphere
    {
        // Get calculted sphere's radius from x-scaling.
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) < 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 2 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) > 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // plane. If the point is on the positive side of the plane, return it.
    else if(clipping_type == 3 && b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        // the origin plane's normal vector is (0, 1, 0). If the dot product of the point and the normal vector is positive, the point is on the positive side of the plane.
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) > 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 3 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) < 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // If the value is out of the legend range, the particle is not drawn.
    if(b_legend_filtering && (aValue < legend_min || aValue > legend_max))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }
    
    vs_out.val_color = aValue;
    vs_out.world_pos = aPos;

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 

)";





/**************************** DPointSingleUintVtk *****************************/


const char* DPointSingleUintVtk_fs = R"(

#version 460
layout (location = 3) out vec4 gPosition; // position + metallic
layout (location = 4) out vec4 gNormal;   // normal + roughness
layout (location = 5) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 6) out vec4 gCustomDepth; // r : opaque, g : fluid. linear depth
layout (location = 7) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    float val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform float legend_min = 0;
uniform float legend_max = 83000;

uniform bool use_legend = false;
uniform mat4 view;
uniform float near;
uniform float far;
uniform float ambient = 0.8;

//const float water_metallic = 0.0;
const float water_metallic = 0.8; // for debugging
//const float water_roughness = 0.01;
const float water_roughness = 0.2; // for debugging 
const float water_IOR = 1.333;
const float water_specular = 0.25;
const uint CUSTOM_MASK_FLUID = 200;

float LinearDepth(float depth)
{
    // Calculate the linear depth from the view matrix and projection matrix
    float z = depth * 2.0 - 1.0; // Convert to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}
// ----------------------------------------------------------------------------
void main()
{   
    vec2 pointUV = gl_PointCoord;
    
    // Calculate normal vector from point coordinates
    vec2 normalPointUV = vec2(pointUV.x, 1.0 - pointUV.y);
    vec3 normal;
    normal.xy = normalPointUV * 2.0 - 1.0;

    // Check if point is within unit circle
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard; // Discard fragments outside the circle

    vec4 tex0_color = texture(tex0, pointUV);

    // Calculate z component using hemisphere formula
    normal.z = sqrt(1.0 - r2) * 2.0 - 1.0;

    // Transform from view space to world space
    vec4 worldNormal = inverse(view) * vec4(normal, 0.0);
    vec3 viewNormalDir = normalize(worldNormal.xyz);

    // Output normal and roughness value
    gNormal = vec4(viewNormalDir, water_roughness);

    //tex1 : legend texture
    float val = fs_in.val_color;
    // calculate val to 0~1 (min, max)
    float val_per = clamp(1.0 - ( (val - legend_min) / (legend_max - legend_min) ), 0.005, 0.995);
    // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
    vec2 legend_color = vec2(val_per, val_per);

//    // if the value is out of the legend range, then the color should be black.
//    if(val < legend_min || val > legend_max)
//    {
//        FragColor = vec4(0.2, 0.2, 0.2, 0.3);
//        return;
//    }

    vec4 tex1_color = texture(tex1, legend_color);
    
    gPosition = vec4(fs_in.world_pos, water_metallic);
    gAlbedoSpec = vec4(tex1_color.rgb * tex0_color.rgb, ambient);
    
    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
    gCustomDepth.g = LinearDepth(gl_FragCoord.z); // linear depth near to far
    gCustomDepth.a = 1.0;

}  

)";



const char* DPointSingleUintVtk_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in uint aValue; 

uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

uniform bool b_clip_invert;
uniform bool b_legend_filtering;
uniform int clipping_type; // 0 - NONE, 1 - CUBE, 2 - SPHERE, 3 - PLANE
uniform mat4 clipping_obj_transform;

uniform float legend_min = 0;
uniform float legend_max = 83000;

out VS_OUT {
    float val_color;
    vec3 world_pos;
} vs_out;

void main()
{
    // cube
    if(clipping_type == 1 && b_clip_invert) // return outside of the cube
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 1 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if ((abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // sphere
    else if(clipping_type == 2 && b_clip_invert) // return outside of the sphere
    {
        // Get calculted sphere's radius from x-scaling.
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) < 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 2 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) > 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // plane. If the point is on the positive side of the plane, return it.
    else if(clipping_type == 3 && b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        // the origin plane's normal vector is (0, 1, 0). If the dot product of the point and the normal vector is positive, the point is on the positive side of the plane.
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) > 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 3 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) < 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }


    float f_value = uintBitsToFloat(aValue);

    // if the value is out of the legend range, then the particle is not drawn.
    if(b_legend_filtering && (f_value < legend_min || f_value > legend_max))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }
    
    vs_out.val_color = f_value;
    vs_out.world_pos = aPos;

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 

)";





/*************************** DynamicComputeDensity ****************************/


const char* DynamicComputeDensity_cs = R"(


#version 460

layout (local_size_x = 256) in;

#define PI_FLOAT 3.1415927410125732421875f
layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[]; // position xyz, density w
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, pressure w
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[]; // acceleration xyz, dummy w
};
uniform float particle_radius;
uniform float smoothing_length = 1.0f;
uniform float particle_mass = 1.0f;
uniform float u_density_0 = 1000.0f;
uniform float u_gas_constant = 2000.0f; // Gas constant for equation of state

// SPH kernel
float Poly6Kernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h * h - r * r);
        return 315.0 / (64.0 * 3.14159265359 * pow(h, 9.0)) * hr * hr * hr;
    } else {
        return 0.0;
    }
}

//// Gradient of the spiky kernel
//float SpikyKernel(float r, float h) {
//    if (r >= 0.0 && r <= h) {
//        float hr = (h - r);
//        return 15.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr * hr;
//    } else {
//        return 0.0;
//    }
//}

// https://arxiv.org/pdf/1706.10263 page 7/26
float KernelFunction(float r, float h)
{
    float s = r / h;
    float section_data = 0.0;

    if ( s >= 0.0 && s < 1.0) 
    {
		section_data =  s * s * s - 6 * s + 6;
	} 
    else if (s >= 1.0 && s < 2.0) {
		section_data =  pow((2-s), 3.0);
	}
    else
        section_data = 0.0;

    return section_data / (3.0 * 3.14159265359 * pow(h, 2.0));
}


float ViscosityKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float r3h3 = - (r * r * r) / (2.0 * (h * h * h));
        float r2h2 = (r * r) / (h * h);
        float r1h1 = h / (2.0 * r);
        return 15.0 / (3.14159265359 * pow(h, 3.0)) * (r3h3 + r2h2 + r1h1 - 1);
    } else {
        return 0.0;
    }
}

void main() {
    uint id = gl_GlobalInvocationID.x;
    if(id >= positions.length()) {
		return;
	}
    if(accelerations[id].w >= -1.8 || accelerations[id].w <= -2.2) {
//    if(accelerations[id].w != -2.0f) {
		return;
	}

    float density_sum = 0.f;

    // Init settings
    float h = smoothing_length;

    // Loop over neighboring cells
    for (uint i = 0; i < positions.length(); ++i) {
        if(accelerations[i].w < -1.8 && accelerations[i].w > -2.2) 
//        if(accelerations[i].w == 2.0)
        {

            vec3 delta = positions[id].xyz - positions[i].xyz;
            float r = distance(positions[id].xyz, positions[i].xyz);
//            float r = length(delta);
			if (r < h) {
			    density_sum += particle_mass * /* poly6 kernel */ 315.f * pow(h * h - r * r, 3) / (64.f * PI_FLOAT * pow(h, 9));
            }
        }
    }

    density_sum = clamp(density_sum, 1.0f, 5000.f);
    float pressure = max(u_gas_constant * (density_sum - u_density_0), 0.f);


    positions[id].w = density_sum;
    velocities[id].w = pressure;
}


)";





/**************************** DynamicComputeForce *****************************/


const char* DynamicComputeForce_cs = R"(

#version 460

layout (local_size_x = 256) in;

#define PI_FLOAT 3.1415927410125732421875f
layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[]; // position xyz, density w
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, pressure w
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[]; // acceleration xyz, dummy w
};

uniform float u_particle_radius;
uniform float u_particle_mass = 1.0f;
uniform float smoothing_length = 1.0f;

//uniform float u_bulk = 1000f;
uniform float u_viscosity = 3000.0f;
uniform float u_density_0 = 1000.0f;
uniform float u_gravity = 9.8f;

// Gradient of the spiky kernel
float SpikyKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float hr = (h - r);
        return 15.0 / (3.14159265359 * pow(h, 6.0)) * hr * hr * hr;
    } else {
        return 0.0;
    }
}

float ViscosityKernel(float r, float h) {
    if (r >= 0.0 && r <= h) {
        float r3h3 = - (r * r * r) / (2.0 * (h * h * h));
        float r2h2 = (r * r) / (h * h);
        float r1h1 = h / (2.0 * r);
        return 15.0 / (3.14159265359 * pow(h, 3.0)) * (r3h3 + r2h2 + r1h1 - 1);
    } else {
        return 0.0;
    }
}

// https://arxiv.org/pdf/1706.10263 page 7/26
float KernelFunction(float r, float h)
{
    float s = r / h;
    float section_data = 0.0;

    if ( s >= 0.0 && s < 1.0) 
    {
		section_data =  s * s * s - 6 * s + 6;
	} 
    else if (s >= 1.0 && s < 2.0) {
		section_data =  pow((2-s), 3.0);
	}
    else
        section_data = 0.0;

    return section_data / (3.0 * 3.14159265359 * pow(h, 2.0));
}

void main() {
    uint id = gl_GlobalInvocationID.x;
    
//    if(accelerations[id].w != -2.0f) {
    if(accelerations[id].w >= -1.8 || accelerations[id].w <= -2.2) {
		return;
	}

    vec3 pos = positions[id].xyz;
//    accelerations[id].xyz = vec3(0.0, 0.0, 0.0);
    const float h = smoothing_length;

    const float viscosity = u_viscosity;
    const float mass = u_particle_mass;

    // Compute Density was done in another compute shader
    vec3 force = vec3(0.0, 0.0, 0.0); // gravity

    const vec3 gravity = vec3(0.0, u_gravity, 0.0);

    // Loop over neighboring cells


    vec3 pressure_force = vec3(0.0, 0.0, 0.0);
    vec3 viscosity_force = vec3(0.0, 0.0, 0.0);

    //  calculate pressure force
    for (uint i = 0; i < positions.length(); ++i) {
        if(accelerations[i].w < -1.8 && accelerations[i].w > -2.2) 
        {
            if (i != id) 
            {
                vec3 delta = pos - positions[i].xyz;
                float r = length(delta);
                if(r == 0.0)
				    continue;
			    if (r < h) {
                    pressure_force -= mass * (velocities[id].w + velocities[i].w) / (2.0f * positions[i].w) *
                    // gradient of spiky kernel
                        -45.f / (PI_FLOAT * pow(h, 6.0)) * pow(h - r, 2.0) * normalize(delta);

                    viscosity_force += mass * (velocities[i].xyz - velocities[id].xyz) / positions[i].w *  
                // Laplacian of viscosity kernel
                    45.f / (PI_FLOAT * pow(h, 6.0)) * (h - r);
                }
            }
        }
    }
    viscosity_force *= viscosity;

    vec3 external_force =  -positions[id].w * gravity;

    //force = pressure_force  + external_force;
    force = pressure_force + viscosity_force + external_force;
    accelerations[id].xyz =  force / positions[id].w;
    // limit the acceleration to 400.
    if(length(accelerations[id].xyz) > 400.0) {
		accelerations[id].xyz = normalize(accelerations[id].xyz) * 400.0;
	}

}


)";





/***************************** DynamicComputePost *****************************/


const char* DynamicComputePost_cs = R"(

#version 460

layout (local_size_x = 256) in;

layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[];
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[];
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[];
};

// vec3 : vertex, float : property. // But now property is not used.
// 3 vertex per triangle. only triangle use.
layout (std430, binding = 5) buffer CollisionMeshBuffer {
    vec4 collision_meshes[];
};

uniform float deltaTime;
uniform vec3 box_min;
uniform vec3 box_max;

uniform vec3 circle_center = vec3(0.0f);
uniform float circle_radius = 1.0f;
uniform float u_particle_radius = 0.1f;

bool CollidePositionTriangle(vec3 pos, vec3 vel, vec3 v0, vec3 v1, vec3 v2) {
    //  Trumbore intersection algorithm
    vec3 pos1 = pos;
    vec3 pos2 = pos + vel * deltaTime;

     vec3 d = pos2 - pos1;
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    vec3 h = cross(d, e2);
    float a = dot(e1, h);

    if (abs(a) < 0.00001) return false;
    float f = 1.0 / a;

    vec3 s = pos1 - v0;
    float u = f * dot(s, h);
    if (u < 0.0 || u > 1.0) return false;

    vec3 q = cross(s, e1);
    float v = f * dot(d, q);
    if (v < 0.0 || u + v > 1.0) return false;

    float t = f * dot(e2, q);

    return (t >= (0.0 - u_particle_radius) && t <= (1.0 + u_particle_radius));

}

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= positions.length()) {
		return;
	}

    if(accelerations[id].w >= 0) {
       positions[id].xyz += velocities[id].xyz * deltaTime;
        return;
	}
    else if(accelerations[id].w >= -1.8 || accelerations[id].w <= -2.2) {
		return;
	}
    const float particle_size = u_particle_radius;

    vec3 pos = positions[id].xyz;
    vec3 vel = velocities[id].xyz;
    vec3 acc = accelerations[id].xyz;

    // Integrate motion


    // Reflect velocity vector when the particle hits the wall
    const float DAMP = 0.3f;



    
	// Collision with the circle, then bouns back
	//if the particle is inside the circle xyz
    vec3 circle_center_to_particle = positions[id].xyz - circle_center;

	if (dot(circle_center_to_particle, circle_center_to_particle) < circle_radius * circle_radius) {
		// if the particle is inside the circle, then move the particle to the surface of the circle
		pos.xyz = circle_center + normalize(circle_center_to_particle) * circle_radius;
		// reflect the velocity vector
		vel.xyz -= 2.0f * dot(vel.xyz, normalize(circle_center_to_particle)) * normalize(circle_center_to_particle) * 1.03f;
	}
	
    
    // Collision with the ground, then bounce back
    if (pos.y < 0.2 + particle_size) {
        pos.y = 0.2 + particle_size;
//         vel.y *= -1 * DAMP;
         //vel.y = - vel.y * DAMP;
//         acc.xyz += 128 * vec3(0, -1, 0) * vel.xyz;

        //p.velocity = p.velocity * -2 * dot(p.velocity, planeNormal) * planeNormal * bounceCoefficient
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 1, 0)) * vec3(0, 1, 0);

    }

    // Collision with the walls, then bounce back
    if (pos.x < box_min.x + particle_size) {
        pos.x = box_min.x + particle_size;

        //p.velocity = p.velocity * -2 * dot(p.velocity, planeNormal) * planeNormal * bounceCoefficient
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(1, 0, 0)) * vec3(1, 0, 0)* DAMP;

        // vel.x = - vel.x * DAMP;
    }
    if (pos.x > box_max.x - particle_size) {
        pos.x = box_max.x - particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(-1, 0, 0)) * vec3(-1, 0, 0)* DAMP;
//         vel.x = - vel.x * DAMP;
    }

    if (pos.y < box_min.y + particle_size) {
	    pos.y = box_min.y + particle_size;
		vel.y = - vel.y * DAMP;
	}
    if (pos.y > box_max.y - particle_size) {
        pos.y = box_max.y - particle_size;
		vel.y = - vel.y * DAMP;
	}

    if (pos.z < box_min.z + particle_size) {
        pos.z = box_min.z + particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 0, 1)) * vec3(0, 0, 1) * DAMP;
//        vel.z = - vel.z * DAMP;
    }
    if (pos.z > box_max.z - particle_size) {
        pos.z = box_max.z - particle_size;
        vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, vec3(0, 0, -1)) * vec3(0, 0, -1) * DAMP;
//        vel.z = - vel.z * DAMP;
    }

    vel.xyz += acc.xyz * deltaTime;
    
    // Triangle Check from collision_meshes
    for (uint collision_index = 0; collision_index< collision_meshes.length(); collision_index += 3) {

        // Get the vertices of the triangle
		vec3 v0 = collision_meshes[collision_index + 0].xyz;
		vec3 v1 = collision_meshes[collision_index + 1].xyz;
		vec3 v2 = collision_meshes[collision_index + 2].xyz;

        // If the position is in the triangle, then bounce back by the plane
		if(CollidePositionTriangle(pos.xyz, vel.xyz, v0, v1, v2))
        {
            // Calculate reflect vector. v = v - 2 * dot(v, n) * n. but the triangle is clockwise or not.
            pos = pos - vel * deltaTime;
            vel.xyz = vel.xyz - 2.0f * dot(vel.xyz, normalize(cross(v1 - v0, v2 - v0))) * normalize(cross(v1 - v0, v2 - v0)) * DAMP;
		}
	}


    pos.xyz += vel.xyz * deltaTime;


    velocities[id].xyz = vel;
    positions[id].xyz = pos;
    
}


)";





/*************************** DynamicInOutFlowBatch ****************************/


const char* DynamicInOutFlowBatch_cs = R"(

#version 450
#extension GL_ARB_shader_atomic_counters : enable
#extension ARB_shader_atomic_counter_ops : enable




layout (local_size_x = 256) in;

const int MAX_CREATED_PARTICLE = 1024;
//struct RSInflowData
//{
//    vec4 inflows[MAX_CREATED_PARTICLE]; 
//	uint current_count;
//	uint max_particle_count;
//	uint inflow_creator_size;
//	uint padding;
//};
struct RSInflowData
{
    vec4 inflows_pos[MAX_CREATED_PARTICLE]; 
	uvec4 inflows_use[MAX_CREATED_PARTICLE]; 

	uint current_count;
	uint max_particle_count;
	uint inflow_creator_size;
	uint padding;
};

layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[];
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, pressure w
};

// acceleration xyz, current zone w.
// w = 0~1023 : in inflow zone
// w = -1 : not in any zone
// w = -2 : in fluid zone
// w = -3 : in outflow zone.
layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[]; 
};

// position xyz, activate w
// w = 0 : not activated. w = 1 : activated.
layout (std430, binding = 6) buffer InflowDataBuffer {
    //vec4 inflows[MAX_CREATED_PARTICLE]; 
	RSInflowData in_flow_data;
};


//uniform vec4 inflow_create_particles[MAX_CREATED_PARTICLE];

uniform vec3 inflow_position;
uniform vec3 inflow_velocity;
uniform vec3 inflow_zone_size;

uniform vec3 outflow_position;
uniform vec3 outflow_zone_size;


//shared uint created_particle_count = 0;
//
//uniform uint to_create_particle_count = 10;

void main() {
    uint id = gl_GlobalInvocationID.x ;

    // 1. check the particle is where by accelerations[].w. 
    // 2. if w = -3, then inactivate the particle. write accelerations[].w = -1. created_particle_count--;
    // 3. if w = -2, then check the particle is in outflow zone. if it is, then write accelerations[].w = -3.
    // 4. if w = 0~1023, then check the particle is in inflow zone. unless it is, then write accelerations[].w = -2.
    //    and inflows[].w = 0, to_create_particle_count++. 
    //
    // 5. if(id > 1023) then continue.
    // if(inflows[id].w == 0) then find to creat a particle from accelerations[].w == -1.
    // 

	// Limit the number of created particles.
    if(id >= positions.length()) {
		return;
	}


	// Check the particle is in inflow zone. Unless it is, write accelerations[].w = -2. 
	// and inflows[].w = 0, to_create_particle_count++.
	if(accelerations[id].w > -0.5 && accelerations[id].w < in_flow_data.inflow_creator_size) {
		if(!(	positions[id].x > inflow_position.x - inflow_zone_size.x * 0.52f && positions[id].x < inflow_position.x + inflow_zone_size.x * 0.52f &&
				positions[id].y > inflow_position.y - inflow_zone_size.y * 0.52f && positions[id].y < inflow_position.y + inflow_zone_size.y * 0.52f &&
				positions[id].z > inflow_position.z - inflow_zone_size.z * 0.52f && positions[id].z < inflow_position.z + inflow_zone_size.z * 0.52f)) {
			uint inflow_id = uint(accelerations[id].w);
//			in_flow_data.inflows_use[inflow_id].x = 0;
//			atomicCompSwap(in_flow_data.inflows_use[id].x, 0, 1);
			accelerations[id].w = -2.0;
			atomicAdd(in_flow_data.inflows_use[inflow_id].x, -1);
			//atomicExchange(in_flow_data.inflows_use[inflow_id].x, 0);
			barrier();
		}
	}
    // Check the particle is where by accelerations[id].w
    else if(accelerations[id].w < -2.5) 
	{
		accelerations[id].w = -1.0;
		//in_flow_data.current_count--;
		atomicAdd(in_flow_data.current_count, -1);
		barrier();
	} 
	// Check the particle is in outflow zone. If it is, write accelerations[].w = -3.
	else if(accelerations[id].w < -1.5) {
		if(positions[id].x > outflow_position.x - outflow_zone_size.x * 0.51f && positions[id].x < outflow_position.x + outflow_zone_size.x * 0.51f &&
		   positions[id].y > outflow_position.y - outflow_zone_size.y * 0.51f && positions[id].y < outflow_position.y + outflow_zone_size.y * 0.51f &&
		   positions[id].z > outflow_position.z - outflow_zone_size.z * 0.51f && positions[id].z < outflow_position.z + outflow_zone_size.z * 0.51f) 
		   {
			accelerations[id].w = -3.0;
//			atomicExchange(accelerations[id].w, -1.0);
	//		atomicAdd(in_flow_data.current_count, -1);
			//in_flow_data.current_count--;
//			accelerations[id].w = -1;
			barrier();
		}
	} 
			barrier();

//	barrier();
//
//	if(in_flow_data.current_count >= in_flow_data.max_particle_count) 
//	{
//		return;
//	}
//
//	if(id < in_flow_data.inflow_creater_size) {
//
////	if(in_flow_data.current_count >= MAX_CREATED_PARTICLE) {
////		return;
////	}
////	barrier();
//			
//
//	if(in_flow_data.inflows[id].w == 0) 
//	{
//		barrier();
//		for(uint i = 0; i < positions.length(); i++) 
//		{
//			if(accelerations[i].w == -1) 
//			{
//				in_flow_data.inflows[id].w = 1;
//				positions[i] = vec4(inflow_position.xyz + in_flow_data.inflows[id].xyz, 1.0);
//				velocities[i] = vec4(inflow_velocity, 0.0);
//				accelerations[i] = vec4(0.0, 0.0, 0.0, id);
//				in_flow_data.current_count++;
//				barrier();
//				return;
//			}
//		}
//
//    }
//		return;
//	}
}

)";





/************************** DynamicInOutFlowCreator ***************************/


const char* DynamicInOutFlowCreator_cs = R"(

//#version 450
//
//layout (local_size_x = 1024) in;
//
//const int MAX_CREATED_PARTICLE = 1024;
//struct RSInflowData
//{
//    vec4 inflows_pos[MAX_CREATED_PARTICLE]; 
//	uvec4 inflows_use[MAX_CREATED_PARTICLE]; 
//
//	uint current_count;
//	uint max_particle_count;
//	uint inflow_creater_size;
//	uint padding;
//};
//
//
//layout (std430, binding = 0) buffer PositionBuffer {
//    vec4 positions[];
//};
//
//layout (std430, binding = 1) buffer VelocityBuffer {
//    vec4 velocities[]; // velocity xyz, pressure w
//};
//
//// acceleration xyz, current zone w.
//// w = 0~1023 : in inflow zone
//// w = -1 : not in any zone
//// w = -2 : in fluid zone
//// w = -3 : in outflow zone.
//layout (std430, binding = 2) buffer AccelerationBuffer {
//    vec4 accelerations[]; 
//};
//
//// position xyz, activate w
//// w = 0 : not activated. w = 1 : activated.
//layout (std430, binding = 6) buffer InflowDataBuffer {
//    //vec4 inflows[MAX_CREATED_PARTICLE]; 
//	RSInflowData in_flow_data;
//};
//
//
//
//uniform vec3 inflow_position;
//uniform vec3 inflow_velocity;
//uniform vec3 inflow_zone_size;
//
//uniform vec3 outflow_position;
//uniform vec3 outflow_zone_size;
//
//void main() {
//    uint id = gl_GlobalInvocationID.x;
//	
//	if(id >= in_flow_data.inflow_creater_size) 
//		return;
//
//	if(in_flow_data.inflows_use[id].x <= 0) 
//	{
//		for(uint i = 0; i < positions.length(); i += 1) 
//		{
//			if(accelerations[i].w == -1) 
//			{
//				in_flow_data.inflows_use[id].x = 1;
//				barrier();
//					
//				if(in_flow_data.current_count >= in_flow_data.max_particle_count) 
//					return;
//				//in_flow_data.inflows_use[id].x = 1;
//				positions[i].xyz = inflow_position + in_flow_data.inflows_pos[id].xyz;
//				velocities[i] = vec4(inflow_velocity, 0.0);
//				accelerations[i] = vec4(0.0, 0.0, 0.0, id);
//				atomicAdd(in_flow_data.current_count, 1);
//				barrier();
//				return;
//			}
//		}
//
//	}
//}
//

///////////////////
#version 450

layout (local_size_x = 1024) in;

const int MAX_CREATED_PARTICLE = 1024;
struct RSInflowData
{
    vec4 inflows_pos[MAX_CREATED_PARTICLE]; 
    uvec4 inflows_use[MAX_CREATED_PARTICLE]; 

    uint current_count;
    uint max_particle_count;
    uint inflow_creator_size;
    uint padding;
};

layout (std430, binding = 0) buffer PositionBuffer {
    vec4 positions[];
};

layout (std430, binding = 1) buffer VelocityBuffer {
    vec4 velocities[]; // velocity xyz, pressure w
};

layout (std430, binding = 2) buffer AccelerationBuffer {
    vec4 accelerations[];
};

layout (std430, binding = 6) buffer InflowDataBuffer {
    RSInflowData in_flow_data;
};

uniform vec3 inflow_position;
uniform vec3 inflow_velocity;
uniform mat4 inflow_rotate_mat;

void main() {
    uint id = gl_GlobalInvocationID.x;
    // id start 1
    if (id >= in_flow_data.inflow_creator_size) 
        return;

    //if (atomicCompSwap(in_flow_data.inflows_use[id].x, 0, 1) == 0) {
    if (in_flow_data.inflows_use[id].x  == 0) {

//        uint current_particle_index = atomicAdd(in_flow_data.current_count, 1);
        if (in_flow_data.current_count >= in_flow_data.max_particle_count) {
            atomicAdd(in_flow_data.current_count, -1);
//            atomicCompSwap(in_flow_data.inflows_use[id].x, 1, 0);
            atomicExchange(in_flow_data.inflows_use[id].x, 0);
            barrier();
            return;
        }

        for (uint i = id; i < positions.length(); i += in_flow_data.inflow_creator_size) {
            if (accelerations[i].w > -1.5 && accelerations[i].w < -0.5) {
                atomicAdd(in_flow_data.current_count, 1);
                atomicExchange(in_flow_data.inflows_use[id].x, 1);
                barrier();
                positions[i].xyz = inflow_position + mat3(inflow_rotate_mat) * in_flow_data.inflows_pos[id].xyz;
                velocities[i] = vec4(inflow_velocity, 0.0);
                accelerations[i] = vec4(0.0, 0.0, 0.0, float(id));
        //        atomicCompSwap(in_flow_data.inflows_use[id].x, 1, 0);
                return;
            }
        }

    }
}

)";





/********************************* HUDPicking *********************************/


const char* HUDPicking_fs = R"(

#version 330 core
out uvec4 FragColor;

in vec2 TexCoords;

uniform uint color_id;
uniform sampler2D tex;

void main() {
    vec2 texcoord_flipy = vec2(TexCoords.x, 1.0 - TexCoords.y);
    vec4 tex_color = texture(tex, texcoord_flipy);
        if(tex_color.a < 0.1) 
    {
        FragColor = uvec4(0, 0, 0, 0);
        return;
    }
    uint result = color_id;
    FragColor = uvec4(result , 0, 0, 0);
}

)";



const char* HUDPicking_vs = R"(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}


)";





/********************************** HudText ***********************************/


const char* HudText_fs = R"(

#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
//    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    // is text
    float is_text = ceil(texture(text, TexCoords).r);
    color = vec4(textColor, is_text);
}

)";



const char* HudText_vs = R"(

#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}

)";





/********************************* HudTexture *********************************/


const char* HudTexture_fs = R"(

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hud_texture;
uniform vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
uniform int flipY = 1;

void main()
{
	// Because the texture is flipped vertically, we need to flip the texture coordinates
	// flipY is 1 true, -1 false. If flipY is true, we flip the texture coordinates. without if condition

	float float_flipy = float(flipY);
	vec2 Texcoord_flipy = float_flipy * vec2( TexCoords.x, 1.0 - TexCoords.y * float_flipy) + (1.0 - float_flipy) * TexCoords;

	FragColor = texture(hud_texture, Texcoord_flipy) * color;
}

)";



const char* HudTexture_vs = R"(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 projection;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = projection * model * vec4(aPos, 1.0);
}

)";





/******************************** MaterialPbr *********************************/


const char* MaterialPbr_fs = R"(

#version 460
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D shadow_map;

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 view_pos;

uniform float opacity = 1.0;
uniform float metallic;
uniform float roughness;
uniform bool use_transparent = false;

uniform mat4 tex_matrix;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
vec3 getNormalFromMap()
{
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);
    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);

    vec3 tangent_normal = texture(normal_map, texcoord_flipy).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangent_normal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 normal, vec3 halfway_dir, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(normal, halfway_dir), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 normal, vec3 view_dir, vec3 light_dir, float roughness)
{
    float NdotV = max(dot(normal, view_dir), 0.0);
    float NdotL = max(dot(normal, light_dir), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
float ShadowCalculation(vec4 FragPosLightSpace)
{
    // perform perspective divide
    vec3 proj_coords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(shadow_map, proj_coords.xy).r; 

    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 light_dir = normalize(light_pos - fs_in.FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, light_dir)), 0.001);
    //float bias = 0.001f;
    // check whether current frag pos is in shadow
    //float shadow = current_depth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x_ = -1; x_ <= 1; ++x_)
    {
        for(int y_ = -1; y_ <= 1; ++y_)
        {
            float pcfDepth = texture(shadow_map, proj_coords.xy + vec2(x_, y_) * texelSize).r; 
//            shadow += current_depth - bias > pcfDepth  ? 1.0 : 0.0;        // hard shadow
            shadow += current_depth - bias > pcfDepth  ? pcfDepth : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
// ----------------------------------------------------------------------------

void main()
{
    // adjust texture coordination
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);

    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);
//    vec2 texcoord_flipy = vec2(fs_in.TexCoords.x, 1.0 - fs_in.TexCoords.y);
    
    vec4 tex_color = texture(diffuse_map, texcoord_flipy);
        if(tex_color.a < 0.1) 
        discard;

    vec3 F0 = vec3(0.04); 
     F0 = mix(F0, color, metallic);
    vec3 normal = getNormalFromMap();
//    vec3 lightColor = vec3(0.5);
    // diffuse
    vec3 light_dir = normalize(light_pos - fs_in.FragPos);
    float diff = max(dot(light_dir, normal), 0.0);
//    vec3 diffuse = diff * lightColor;
    // specular
    vec3 view_dir = normalize(view_pos - fs_in.FragPos);
//    vec3 reflectDir = reflect(-light_dir, normal);
    vec3 halfway_dir = normalize(light_dir + view_dir);  

    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, halfway_dir, roughness);   
    float G   = GeometrySmith(normal, view_dir, light_dir, roughness);      
    vec3 F    = fresnelSchlick(clamp(dot(halfway_dir, view_dir), 0.0, 1.0), F0);
           
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(normal, view_dir), 0.0) * max(dot(normal, light_dir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
        
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  

    // calculate shadow

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    
    // scale light by NdotL
    float NdotL = max(dot(normal, light_dir), 0.0);        

    // add to outgoing radiance Lo

    vec3 lighting = (kD * color / PI + specular) * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    //(ambient + (1.0 - shadow) ) * tex_color.xyz * color;    

    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * tex_color.xyz * color;    
    
    
    // ambient
    vec3 ambient = vec3(0.2f);

    vec3 final_color = (ambient + (1.0 - shadow) * lighting) * tex_color.xyz * color;    
//    vec3 final_color = ambient + lighting;
    
    // HDR tonemapping
    final_color = final_color / (final_color + vec3(1.0));
    // gamma correct
    final_color = pow(final_color, vec3(1.0/2.2)); 

	FragColor = vec4(final_color, 1.0);
    if(use_transparent)
    {
		FragColor = vec4(tex_color.xyz* color, opacity);
    }
}

)";



const char* MaterialPbr_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//uniform vec3 light_dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_matrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
//    vs_out.Normal = mat3(model) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = light_space_matrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);

}



)";





/**************************** MaterialPbr_gbuffer *****************************/


const char* MaterialPbr_gbuffer_fs = R"(

#version 460
layout (location = 0) out vec4 gPosition; // position + metallic
layout (location = 1) out vec4 gNormal;   // normal + roughness
layout (location = 2) out vec4 gAlbedoSpec; // albedo + dummy(nothing)
layout (location = 3) out uvec4 gCustomIndex; // mask, nothing 3

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;

uniform vec3 color;

uniform float metallic;
uniform float roughness;
uniform float ambient = 0.3;

uniform mat4 tex_matrix;

const uint CUSTOM_MASK_OPAQUE = 100;

// ----------------------------------------------------------------------------
vec3 GetNormalFromMap()
{
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);
    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);

    vec3 tangent_normal = texture(normal_map, texcoord_flipy).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangent_normal);
}
// ----------------------------------------------------------------------------
void main()
{
    // adjust texture coordination
    vec4 texcoord = tex_matrix * vec4(fs_in.TexCoords, 0.0, 1.0);

    vec2 texcoord_flipy = vec2(texcoord.x, 1.0 - texcoord.y);
    
    vec4 tex_color = texture(diffuse_map, texcoord_flipy);
        if(tex_color.a < 0.1) 
        discard;

    gPosition.xyz = fs_in.FragPos;
    gPosition.w = metallic;

    gNormal.xyz = GetNormalFromMap();
    gNormal.w = roughness;

    gAlbedoSpec.xyz = tex_color.rgb * color;
    gAlbedoSpec.w = ambient;

    gCustomIndex = uvec4(CUSTOM_MASK_OPAQUE); // nothing
//    gCustomIndex.r = CUSTOM_MASK_OPAQUE; // Opaque mask
}

)";



const char* MaterialPbr_gbuffer_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//uniform vec3 light_dir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

void main()
{
    vec4 world_pos = model * vec4(aPos, 1.0);
    vs_out.FragPos = world_pos.xyz;
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;

    gl_Position = projection * view * world_pos ;

}



)";





/*************************** ParticleDefaultShader ****************************/


const char* ParticleDefaultShader_fs = R"(

#version 430 core

in vec3 val_color;
in vec3 world_pos;
out vec4 FragColor;


uniform mat4 view;
uniform mat4 projection;


uniform vec4 point_min_point_type = vec4(0, 0, 0, 0);
uniform vec4 vector_max_radius_inout = vec4(0, 0, 0, 1);

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform bool use_legend = false;
uniform bool use_texture_second = false;

// Should get min/max value
uniform float max_value = 10.0;
uniform float min_value = 0.0;
int clip_plane(vec3 point, vec3 normal, vec3 world_pos) {
	return (dot(world_pos - point, normal) < 0) ? 1 : -1;
}

int clip_box(vec3 min, vec3 max, vec3 world_pos) {
	return (world_pos.x < min.x || world_pos.x > max.x || world_pos.y < min.y || world_pos.y > max.y || world_pos.z < min.z || world_pos.z > max.z) ? -1 : 1;
}

int clip_sphere(vec3 center, float radius, vec3 world_pos) {
	return (length(world_pos - center) < radius) ? 1 : -1;
}


void main() {
    // calculate clipping
    switch (int(point_min_point_type.w)) {
        // plane clipping case
		case 1:
            // calculate that world position is above the plane which is defined with 3d point and 3d vector.
            // position = point_min_point_type.xyz
            // vector = vector_max_radius_inout.xyz
            if(clip_plane(point_min_point_type.xyz, vector_max_radius_inout.xyz, world_pos) * vector_max_radius_inout.w < 0) {
				discard;
			}
			break;

            // cube min max clipping case
		case 2:
            // calculate that world position is inside the box which is defined with min point and max point.
            // min = point_min_point_type.xyz
            // max = vector_max_radius_inout.xyz
            if(clip_box(point_min_point_type.xyz, vector_max_radius_inout.xyz, world_pos) * vector_max_radius_inout.w < 0) {
				discard;
			}
			break;

            // sphere clipping case
		case 3:
            // calculate that world position is inside the sphere which is defined with center point and radius.
            // center = point_min_point_type.xyz
            // radius = vector_max_radius_inout.x
            if(clip_sphere(point_min_point_type.xyz, vector_max_radius_inout.x, world_pos) * vector_max_radius_inout.w < 0) {
				discard;
			}
			break;
        default : 
            break;
	}



    // calculate legend texture color
    if(use_legend) {
    // if out of circle, discard
        vec2 pointUV = gl_PointCoord;
        if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
            discard;

        float val = length(val_color);
        // calculate val to 0~1 (min, max)
        float val_per = clamp(1.0 - ((val - min_value) / (max_value - min_value)), 0.005, 0.995);
        // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
        vec2 legend_color = vec2(val_per, val_per);
        //vec4 tex0_color = texture(tex0, legend_color);
        vec4 tex0_color = texture(tex0, legend_color);

        vec4 tex1_color = vec4(1.0, 1.0, 1.0, 1.0);

        if(use_texture_second)
        {
            tex1_color = texture(tex1, pointUV);

            if(tex1_color.a < 0.1) discard;

        }
        FragColor = tex0_color * tex1_color;
//        FragColor.a = 0.1;
        //FragColor = tex0_color;
       return;
    }
    else{
        //  val_color is -10 ~ 10. convert to 0~1
        vec2 pointUV = gl_PointCoord;
        if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
            discard;
        vec3 satured_color = val_color * 0.05 + 0.5;
    //    vec3 satured_color = val_color;
    //    satured_color = abs(satured_color.xyz);
        satured_color.x += 0.1;

        // 

        satured_color.z += 0.2;
        FragColor = vec4(satured_color.yxz + 0.2, 1.0);
    //    FragColor = vec4(satured_color.xyz, 1.0);
    //    FragColor = vec4(0, 0, 1, 1.0);
    }
}

)";



const char* ParticleDefaultShader_vs = R"(

#version 430 core

layout(std430, binding = 0) buffer PositionOut {
    vec4 positions[];
};

layout(std430, binding = 1) buffer VelocityOut {
    vec4 velocities[];
};

layout(std430, binding = 2) buffer AccelerationOut {
	vec4 accelerations[];
};

layout(location = 0) in uint instanceID;

out vec3 val_color;
out vec3 world_pos;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_position;
// 0 - none
// 1 - velocity
// 2 - acceleration
// 3 - density (position.w)
// 4 - pressure (velocity.w)
uniform int color_type = 0;

float screen_size = 1024.0f;

void main() {
	if(accelerations[instanceID].w == -1.0)
	{
		gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		return;
	}

    val_color = vec3(0.0);
    switch(color_type) {
		case 0:
			val_color = vec3(velocities[instanceID].xyz);
			break;
		case 1:
			val_color = vec3(velocities[instanceID].xyz);
			break;
		case 2:
			val_color = vec3(accelerations[instanceID].xyz);
			break;
		case 3:
			val_color = vec3(positions[instanceID].w, 0, 0);
			break;
		case 4:
			val_color = vec3(velocities[instanceID].w, 0, 0);
			break;
		default:
			val_color = vec3(1.0);
			break;
	}




    vec3 pos = positions[instanceID].xyz;
    gl_Position = projection * view * vec4(pos, 1.0);
    world_pos = pos;

    float distance_from_camera = max(length(camera_position - pos), 0.001);

    //gl_Position = vec4(pos, 1.0);
    gl_PointSize = screen_size / distance_from_camera;
}

)";





/************************ ParticleDefaultShader_backup ************************/


const char* ParticleDefaultShader_backup_fs = R"(

#version 430 core

in vec3  val_color;
out vec4 FragColor;


uniform mat4 view;
uniform mat4 projection;

uniform sampler2D tex;
uniform bool use_texture = false;

void main() {
    // calculate legend texture color
    if(use_texture) {
        float val = length(val_color);
        // val is -10 ~ 10. convert to 0~1 about texture x-axis coordinate
        vec2 val_color = vec2(1.0 - val * 0.15);
        FragColor = texture(tex, val_color);
    }
    else{
        //  val_color is -10 ~ 10. convert to 0~1
        vec3 satured_color = val_color * 0.05 + 0.5;
    //    vec3 satured_color = val_color;
    //    satured_color = abs(satured_color.xyz);
        satured_color.x += 0.1;

        // 

        satured_color.z += 0.2;
        FragColor = vec4(satured_color.yxz + 0.2, 1.0);
    //    FragColor = vec4(satured_color.xyz, 1.0);
    //    FragColor = vec4(0, 0, 1, 1.0);
    }
}

)";





/****************************** PointCloudShader ******************************/


const char* PointCloudShader_fs = R"(

#version 460

uniform sampler2D tex0;

in vec4 out_color;

out vec4 FragColor;

uniform int clipping_id = 0;
uniform vec3 clipping_color;

void main()
{   
//    if(clipping_id == 1){
//    	if(out_color.x > clipping_color.x || out_color.y > clipping_color.y || out_color.z > clipping_color.z) {
//		discard;
//	    }
//	}

    vec2 pointUV = gl_PointCoord;
    if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
        discard;
    vec4 tex1_color = texture(tex0, pointUV);
//    if(tex1_color.a < 0.99)
//        discard;
    FragColor = out_color * tex1_color;

//    FragColor = out_color;
}  

)";



const char* PointCloudShader_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor; 

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float point_size;
uniform int clipping_id = 0;
uniform mat4 box_transform;

out vec4 out_color;

void main()
{
    vec4 color = vec4(aColor, 1.0);
    // if aPos is outside the box, discard the point.
    // The box is 1x1x1 centered at the origin. And the box_transform is the transformation matrix that scales and translates and rotates the box.
    if(clipping_id == 1)
    {
        vec4 pos = inverse(box_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
	    {
		    gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		    return;
	    }
    }
    else if(clipping_id == 0)
    {
        vec4 pos = inverse(box_transform) * vec4(aPos, 1.0);
        if (abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5)
	    {
            color = vec4(0.2, 0.2, 0.2, 0.3);
	    }
    }


    out_color = color;
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 

)";





/***************************** PointMultiFloatVtk *****************************/


const char* PointMultiFloatVtk_fs = R"(

#version 460

uniform sampler2D tex0;
uniform vec3 value_min;
uniform vec3 value_max;

in vec4 out_color;

out vec4 FragColor;


void main()
{   
    vec2 pointUV = gl_PointCoord;
    if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
        discard;

    vec4 tex1_color = texture(tex0, pointUV);
//    if(tex1_color.a < 0.99)
//        discard;

    vec4 value_color = out_color;
    // value_color should changed 0~1 by value_min~value_max for xyz
    vec3 min_max_diff = value_max - value_min;

    // When any element of diff is 0, then the element should be 0.1.
    min_max_diff = max(min_max_diff, vec3(0.1, 0.1, 0.1));

    value_color.rgb = (value_color.rgb - value_min) / min_max_diff;

//    // if any the value is out of the value_min and value_max, then the color should be black.
//    if(value_color.r < 0.0 || value_color.r > 1.0 || value_color.g < 0.0 || value_color.g > 1.0 || value_color.b < 0.0 || value_color.b > 1.0)
//    {
//        value_color = vec4(0.2, 0.2, 0.2, 0.3);
//    }


    FragColor = value_color * tex1_color;

//    FragColor = out_color;
}  

)";



const char* PointMultiFloatVtk_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor; 

uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

uniform bool b_clip_invert;
uniform bool b_legend_filtering;
uniform int clipping_type; // 0 - NONE, 1 - CUBE, 2 - SPHERE, 3 - PLANE
uniform mat4 clipping_obj_transform;

uniform vec3 value_min;
uniform vec3 value_max;

out vec4 out_color;

void main()
{
// cube
    if(clipping_type == 1 && b_clip_invert) // return outside of the cube
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 1 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if ((abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // sphere
    else if(clipping_type == 2 && b_clip_invert) // return outside of the sphere
    {
        // Get calculted sphere's radius from x-scaling.
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) < 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 2 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) > 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // plane. If the point is on the positive side of the plane, return it.
    else if(clipping_type == 3 && b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        // the origin plane's normal vector is (0, 1, 0). If the dot product of the point and the normal vector is positive, the point is on the positive side of the plane.
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) > 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 3 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) < 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // If the point is outside of the legend range, the particle is not drawn.
    if(b_legend_filtering && (aColor.x < value_min.x || aColor.x > value_max.x || aColor.y < value_min.y || aColor.y > value_max.y || aColor.z < value_min.z || aColor.z > value_max.z))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }

    vec4 color = vec4(aColor, 1.0);

    out_color = color;
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 

)";





/**************************** PointSingleFloatVtk *****************************/


const char* PointSingleFloatVtk_fs = R"(

#version 460

uniform sampler2D tex0;
uniform sampler2D tex1;

in float out_value;

out vec4 FragColor;

uniform float legend_min = 0;
uniform float legend_max = 10;

uniform bool use_legend = false;

void main()
{   
    vec2 pointUV = gl_PointCoord;
    if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
        discard;

    vec4 tex0_color = texture(tex0, pointUV);

    //tex1 : legend texture
    float val = out_value;
    // calculate val to 0~1 (min, max)
    float val_per = clamp(1.0 - ( float(val - legend_min) / float(legend_max - legend_min)), 0.005, 0.995);
    // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
    vec2 legend_color = vec2(val_per, val_per);

//    // if the value is out of the legend range, then the color should be black.
//    if(val < legend_min || val > legend_max)
//    {
//        FragColor = vec4(0.2, 0.2, 0.2, 0.3);
//        return;
//    }

    vec4 tex1_color = texture(tex1, legend_color);


    FragColor = tex0_color * tex1_color;

}  

)";



const char* PointSingleFloatVtk_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in float aValue; 

uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

uniform bool b_clip_invert;
uniform bool b_legend_filtering;
uniform int clipping_type; // 0 - NONE, 1 - CUBE, 2 - SPHERE, 3 - PLANE
uniform mat4 clipping_obj_transform;

uniform float legend_min = 0;
uniform float legend_max = 10;

out float out_value;

void main()
{
// cube
    if(clipping_type == 1 && b_clip_invert) // return outside of the cube
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 1 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if ((abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // sphere
    else if(clipping_type == 2 && b_clip_invert) // return outside of the sphere
    {
        // Get calculted sphere's radius from x-scaling.
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) < 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 2 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) > 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // plane. If the point is on the positive side of the plane, return it.
    else if(clipping_type == 3 && b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        // the origin plane's normal vector is (0, 1, 0). If the dot product of the point and the normal vector is positive, the point is on the positive side of the plane.
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) > 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 3 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) < 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // If the value is out of the legend range, the particle is not drawn.
    if(b_legend_filtering && (aValue < legend_min || aValue > legend_max))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
    out_value = aValue;
} 

)";





/***************************** PointSingleUintVtk *****************************/


const char* PointSingleUintVtk_fs = R"(

#version 460

uniform sampler2D tex0;
uniform sampler2D tex1;

in float out_value;

out vec4 FragColor;

uniform float legend_min = 0;
uniform float legend_max = 83000;

uniform bool use_legend = false;

void main()
{   
    vec2 pointUV = gl_PointCoord;
    if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
        discard;

    vec4 tex0_color = texture(tex0, pointUV);

    //tex1 : legend texture
    float val = out_value;
    // calculate val to 0~1 (min, max)
    float val_per = clamp(1.0 - ( (val - legend_min) / (legend_max - legend_min) ), 0.005, 0.995);
    // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
    vec2 legend_color = vec2(val_per, val_per);

//    // if the value is out of the legend range, then the color should be black.
//    if(val < legend_min || val > legend_max)
//    {
//        FragColor = vec4(0.2, 0.2, 0.2, 0.3);
//        return;
//    }

    vec4 tex1_color = texture(tex1, legend_color);

    FragColor = tex0_color * tex1_color;

}  

)";



const char* PointSingleUintVtk_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in uint aValue; 

uniform mat4 view;
uniform mat4 projection;
uniform float point_size;

uniform bool b_clip_invert;
uniform bool b_legend_filtering;
uniform int clipping_type; // 0 - NONE, 1 - CUBE, 2 - SPHERE, 3 - PLANE
uniform mat4 clipping_obj_transform;

uniform float legend_min = 0;
uniform float legend_max = 83000;

out float out_value;

void main()
{
    // cube
    if(clipping_type == 1 && b_clip_invert) // return outside of the cube
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 1 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if ((abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // sphere
    else if(clipping_type == 2 && b_clip_invert) // return outside of the sphere
    {
        // Get calculted sphere's radius from x-scaling.
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) < 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 2 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (length(pos.xyz) > 0.5)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }

    // plane. If the point is on the positive side of the plane, return it.
    else if(clipping_type == 3 && b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        // the origin plane's normal vector is (0, 1, 0). If the dot product of the point and the normal vector is positive, the point is on the positive side of the plane.
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) > 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }
    else if(clipping_type == 3 && !b_clip_invert)
    {
        vec4 pos = inverse(clipping_obj_transform) * vec4(aPos, 1.0);
        if (dot(pos, vec4(0.0, 1.0, 0.0, 0.0)) < 0.0)
        {
            gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
            return;
        }
    }


    float f_value = uintBitsToFloat(aValue);

    // if the value is out of the legend range, then the particle is not drawn.
    if(b_legend_filtering && (f_value < legend_min || f_value > legend_max))
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }


    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
    out_value = f_value;
} 

)";





/******************************* PointVtkRender *******************************/


const char* PointVtkRender_fs = R"(

#version 460

uniform sampler2D tex0;

in vec4 out_color;

out vec4 FragColor;

uniform int clipping_id = 0;
uniform vec3 clipping_color;

void main()
{   
//    if(clipping_id == 1){
//    	if(out_color.x > clipping_color.x || out_color.y > clipping_color.y || out_color.z > clipping_color.z) {
//		discard;
//	    }
//	}

    vec2 pointUV = gl_PointCoord;
    if(length(pointUV - vec2(0.5, 0.5)) > 0.5) 
        discard;
    vec4 tex1_color = texture(tex0, pointUV);
//    if(tex1_color.a < 0.99)
//        discard;
    FragColor = out_color * tex1_color;

//    FragColor = out_color;
}  

)";



const char* PointVtkRender_vs = R"(

#version 460
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor; 

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float point_size;
uniform int clipping_id = 0;
uniform mat4 box_transform;

out vec4 out_color;

void main()
{
    vec4 color = vec4(aColor, 1.0);
    // if aPos is outside the box, discard the point.
    // The box is 1x1x1 centered at the origin. And the box_transform is the transformation matrix that scales and translates and rotates the box.
    if(clipping_id == 1)
    {
        vec4 pos = inverse(box_transform) * vec4(aPos, 1.0);
        if (!(abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5))
	    {
		    gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		    return;
	    }
    }
    else if(clipping_id == 0)
    {
        vec4 pos = inverse(box_transform) * vec4(aPos, 1.0);
        if (abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5)
	    {
            color = vec4(0.2, 0.2, 0.2, 0.3);
	    }
    }


    out_color = color;
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = point_size / gl_Position.w;
} 

)";





/******************************* Point_gbuffer ********************************/


const char* Point_gbuffer_fs = R"(

#version 460
layout (location = 0) out vec4 gPosition; // position + metallic
layout (location = 1) out vec4 gNormal;   // normal + roughness
layout (location = 2) out vec4 gAlbedoSpec; // albedo + ao
layout (location = 3) out uvec4 gCustomIndex; // mask, nothing 3
//layout (location = 3) out vec4 gDepth;

in VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} fs_in;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec4 point_min_point_type = vec4(0, 0, 0, 0);
uniform vec4 vector_max_radius_inout = vec4(0, 0, 0, 1);

uniform bool use_legend = false;
uniform bool use_texture_second = false;

// Should get min/max value
uniform float max_value = 10.0;
uniform float min_value = 0.0;

uniform mat4 view;
uniform mat4 projection;

uniform float ambient = 0.8;

// Water INFO
// metallic = 0.0
// roughness = 0.01
// IOR = 1.333
// Specular = 0.25

//const float water_metallic = 0.0;
const float water_metallic = 0.8; // for debugging
//const float water_roughness = 0.01;
const float water_roughness = 0.2; // for debugging 
const float water_IOR = 1.333;
const float water_specular = 0.25;

const uint CUSTOM_MASK_FLUID = 200;


// ----------------------------------------------------------------------------
int clip_plane(vec3 point, vec3 normal, vec3 world_pos) {
	return (dot(world_pos - point, normal) < 0) ? 1 : -1;
}
// ----------------------------------------------------------------------------
int clip_box(vec3 min, vec3 max, vec3 world_pos) {
	return (world_pos.x < min.x || world_pos.x > max.x || world_pos.y < min.y || world_pos.y > max.y || world_pos.z < min.z || world_pos.z > max.z) ? -1 : 1;
}
// ----------------------------------------------------------------------------
int clip_sphere(vec3 center, float radius, vec3 world_pos) {
	return (length(world_pos - center) < radius) ? 1 : -1;
}
// ----------------------------------------------------------------------------
void main()
{
 // calculate clipping
 switch (int(point_min_point_type.w)) {
    // plane clipping case
	case 1:
        // calculate that world position is above the plane which is defined with 3d point and 3d vector.
        // position = point_min_point_type.xyz
        // vector = vector_max_radius_inout.xyz
        if(clip_plane(point_min_point_type.xyz, vector_max_radius_inout.xyz, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;

        // cube min max clipping case
	case 2:
        // calculate that world position is inside the box which is defined with min point and max point.
        // min = point_min_point_type.xyz
        // max = vector_max_radius_inout.xyz
        if(clip_box(point_min_point_type.xyz, vector_max_radius_inout.xyz, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;

        // sphere clipping case
	case 3:
        // calculate that world position is inside the sphere which is defined with center point and radius.
        // center = point_min_point_type.xyz
        // radius = vector_max_radius_inout.x
        if(clip_sphere(point_min_point_type.xyz, vector_max_radius_inout.x, fs_in.world_pos) * vector_max_radius_inout.w < 0) {
			discard;
		}
		break;
    default : 
        break;
    }
    // Calculate normal from point

    // if out of circle, discard
    vec2 pointUV = gl_PointCoord;

    vec3 normal;
    vec2 normal_point_uv = pointUV;
    normal_point_uv.y = 1.0 - normal_point_uv.y;
    normal.xy = normal_point_uv * 2.0 - 1.0;
    float r2 = dot(normal.xy, normal.xy);
    if (r2 > 1.0) discard;
    normal.z = (sqrt(1.0 - r2)) * 2.0 - 1.0;

    vec4 world_normal = inverse(view) * vec4(normal, 0.0);
    vec3 view_normal_dir = normalize(world_normal.xyz);
    gNormal = vec4(view_normal_dir, water_roughness);
    gPosition = vec4(fs_in.world_pos, water_metallic);

    gCustomIndex = uvec4(CUSTOM_MASK_FLUID); // nothing
//    gCustomIndex.r = CUSTOM_MASK_FLUID; // Opaque mask


    // calculate legend texture color
    if(use_legend) {

        float val = length(fs_in.val_color);
        // calculate val to 0~1 (min, max)
        float val_per = clamp(1.0 - ((val - min_value) / (max_value - min_value)), 0.005, 0.995);
        // val is 0 ~ max_val. convert to 0~1 about texture x-axis coordinate
        vec2 legend_color = vec2(val_per, val_per);
        //vec4 tex0_color = texture(tex0, legend_color);
        vec4 tex0_color = texture(tex0, legend_color);

        vec4 tex1_color = vec4(1.0, 1.0, 1.0, ambient);

        if(use_texture_second)
        {
            tex1_color = texture(tex1, pointUV);

            if(tex1_color.a < 0.1) discard;
        }
        gAlbedoSpec = tex0_color * tex1_color;
       return;
    }
    else{
    //  val_color is -10 ~ 10. convert to 0~1
    vec3 satured_color = fs_in.val_color * 0.05 + 0.5;
    //gAlbedoSpec = vec4(satured_color, 1.0f);
    gAlbedoSpec = vec4(0.824, 0.91, 0.93, ambient);

    }

}

)";



const char* Point_gbuffer_vs = R"(

#version 460
layout(std430, binding = 0) buffer PositionOut {
    vec4 positions[];
};

layout(std430, binding = 1) buffer VelocityOut {
    vec4 velocities[];
};

layout(std430, binding = 2) buffer AccelerationOut {
	vec4 accelerations[];
};

layout(location = 0) in uint instanceID;


out VS_OUT {
    vec3 val_color;
    vec3 world_pos;
} vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_position;
// 0 - none
// 1 - velocity
// 2 - acceleration
// 3 - density (position.w)
// 4 - pressure (velocity.w)
uniform int color_type = 0;

float screen_size = 1024.0f;

void main()
{
	if(accelerations[instanceID].w == -1.0)
	{
		gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
		return;
	}

    vs_out.val_color = vec3(0.0);
    switch(color_type) {
		case 0:
			vs_out.val_color = vec3(velocities[instanceID].xyz);
			break;
		case 1:
			vs_out.val_color = vec3(velocities[instanceID].xyz);
			break;
		case 2:
			vs_out.val_color = vec3(accelerations[instanceID].xyz);
			break;
		case 3:
			vs_out.val_color = vec3(positions[instanceID].w, 0, 0);
			break;
		case 4:
			vs_out.val_color = vec3(velocities[instanceID].w, 0, 0);
			break;
		default:
			vs_out.val_color = vec3(1.0);
			break;
	}




    vec3 pos = positions[instanceID].xyz;
    gl_Position = projection * view * vec4(pos, 1.0);
    vs_out.world_pos = pos;

    float distance_from_camera = max(length(camera_position - pos), 0.001);

    //gl_Position = vec4(pos, 1.0);
    gl_PointSize = screen_size / distance_from_camera;

}



)";





/****************************** PostColorFilter *******************************/


const char* PostColorFilter_fs = R"(

#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D input_texture;

// UBO for color filter data (std140 layout)
// Total: 32 bytes (2 x vec4)
layout(std140, binding = 1) uniform ColorFilterData
{
    vec4 weights_intensity;  // weights.xyz, intensity
    vec4 params;             // mode, padding, padding, padding
};

void main()
{
    vec3 color = texture(input_texture, TexCoords).rgb;
    vec3 weights = weights_intensity.xyz;
    float intensity = weights_intensity.w;
    int mode = int(params.x);
    
    vec3 result = color;
    
    // Mode: 0=None, 1=Grayscale, 2=Red, 3=Green, 4=Blue, 5=Sepia, 6=Invert, 7=Custom
    if (mode == 1) // Grayscale
    {
        float gray = dot(color, weights);
        result = vec3(gray);
    }
    else if (mode == 2) // Red channel only
    {
        result = vec3(color.r, 0.0, 0.0);
    }
    else if (mode == 3) // Green channel only
    {
        result = vec3(0.0, color.g, 0.0);
    }
    else if (mode == 4) // Blue channel only
    {
        result = vec3(0.0, 0.0, color.b);
    }
    else if (mode == 5) // Sepia
    {
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        result = vec3(gray * weights.x, gray * weights.y, gray * weights.z);
    }
    else if (mode == 6) // Invert
    {
        result = vec3(1.0) - color;
    }
    else if (mode == 7) // Custom - use weights as multipliers
    {
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        result = vec3(gray) * weights;
    }
    
    // Blend between original and filtered based on intensity
    result = mix(color, result, intensity);
    
    // Clamp result
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}


)";





/****************************** PostKernelFilter ******************************/


const char* PostKernelFilter_fs = R"(

#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D input_texture;

// UBO for kernel data (std140 layout)
// std140 alignment: each array element is aligned to vec4 (16 bytes)
layout(std140, binding = 0) uniform KernelData
{
    vec4 kernel_row0;  // kernel[0], kernel[1], kernel[2], padding
    vec4 kernel_row1;  // kernel[3], kernel[4], kernel[5], padding
    vec4 kernel_row2;  // kernel[6], kernel[7], kernel[8], padding
    vec4 params;       // divisor, offset, padding, padding
};

void main()
{
    vec2 texel_size = 1.0 / textureSize(input_texture, 0);
    
    vec3 result = vec3(0.0);
    
    float divisor = params.x;
    float offset_val = params.y;
    
    // 3x3 convolution kernel application
    // Kernel layout:
    // [0][1][2]  -> kernel_row0.xyz
    // [3][4][5]  -> kernel_row1.xyz
    // [6][7][8]  -> kernel_row2.xyz
    
    // Top row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, -texel_size.y)).rgb * kernel_row0.x;
    result += texture(input_texture, TexCoords + vec2(0.0,           -texel_size.y)).rgb * kernel_row0.y;
    result += texture(input_texture, TexCoords + vec2( texel_size.x, -texel_size.y)).rgb * kernel_row0.z;
    
    // Middle row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, 0.0)).rgb * kernel_row1.x;
    result += texture(input_texture, TexCoords + vec2(0.0,           0.0)).rgb * kernel_row1.y;
    result += texture(input_texture, TexCoords + vec2( texel_size.x, 0.0)).rgb * kernel_row1.z;
    
    // Bottom row
    result += texture(input_texture, TexCoords + vec2(-texel_size.x, texel_size.y)).rgb * kernel_row2.x;
    result += texture(input_texture, TexCoords + vec2(0.0,           texel_size.y)).rgb * kernel_row2.y;
    result += texture(input_texture, TexCoords + vec2( texel_size.x, texel_size.y)).rgb * kernel_row2.z;
    
    // Apply divisor and offset
    result = result / divisor + offset_val;
    
    // Clamp result to valid range
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}


)";





/******************************** QuadTexture *********************************/


const char* QuadTexture_fs = R"(

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D quad_texture;

void main() {
    vec4 quad_color = texture(quad_texture, TexCoords);
        if(quad_color.a < 0.1) discard;
    FragColor = quad_color;  // Simple direct texturing, blend in application
}

)";



const char* QuadTexture_vs = R"(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

)";





/******************************* QuadTextureR32 *******************************/


const char* QuadTextureR32_fs = R"(

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform usampler2D quad_r32_texture;

void main() {
    uint data = texture(quad_r32_texture, TexCoords).r; // Retrieve the integer data
    if(data == 0u) // If the data is 0, it means the pixel is empty
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Grayscale representation
		discard;
    }
    float intensity = float(data) / 4294967295.0; // Normalize assuming 32-bit max value
    FragColor = vec4(intensity, 0.0, 0.0, 1.0); // Grayscale representation
}

)";



const char* QuadTextureR32_vs = R"(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

)";





/********************************* Quad_Debug *********************************/


const char* Quad_Debug_fs = R"(

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depth_map;
uniform float near_plane;
uniform float far_plane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{             
    float depthValue = texture(depth_map, TexCoords).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}

)";



const char* Quad_Debug_vs = R"(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

)";





/******************************* ShadowMapping ********************************/


const char* ShadowMapping_fs = R"(

#version 330 core
out vec4 FragColor;

void main()
{   
    gl_FragDepth = gl_FragCoord.z;
}

)";



const char* ShadowMapping_vs = R"(

#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 light_space_matrix;
uniform mat4 model;

void main()
{
    gl_Position = light_space_matrix * model * vec4(aPos, 1.0);
}

)";





/*********************************** Skybox ***********************************/


const char* Skybox_fs = R"(

#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float alpha;

void main()
{   
    
    FragColor = texture(skybox, TexCoords);
//    FragColor.a = clamp(alpha, 0.0, 0.5);
}

)";



const char* Skybox_vs = R"(

#version 330 core
layout (location = 0) in vec3 vertex;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = vertex;
	vec4 pos = projection * view * vec4(vertex, 1.0);
	gl_Position = pos.xyww;
}

)";





/********************************* SolidColor *********************************/


const char* SolidColor_fs = R"(

#version 330 core
out vec4 color;

uniform vec4 solid_color;

void main()
{    
	color = vec4(solid_color);
}

)";



const char* SolidColor_vs = R"(

#version 330 core
layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
}

)";




