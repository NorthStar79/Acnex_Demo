#include <acknex.h>
#include <default.c>
#include <stdio.h>
#include "asPathfinder.h"

BMAP *crosshair_pcx = "crosshair.pcx";
BMAP *healthBar = "healthBar.jpg";
ENTITY *playerEnt;


// Add waypoint to node list
action AddWaypoint()
{
	set(me, PASSABLE);
	asNode newnode;
	asZeroNode(newnode);
	vec_set(newnode.pos, my.x);
	asID += 1;
	newnode.ID = asID;
	asPrepend(asNodeList, newnode);
}

PANEL *health_bar =
{
	BMAP = healthBar;
  size_x = 500;
  size_y = 32;
  
  flags = SHOW;
}

PANEL *gameOver_panel =
{
	size_x = 0; 
  	size_y = 0;
 	red = 0;
  	green = 0;
  	blue = 0;
  	flags = LIGHT | SHOW;
}

function player_hit()
{
	playerEnt.skill10 -=10;
	if(playerEnt.skill10  <= 0)
	{
		//game over;
		playerEnt.skill1 =0;
	}
	
	health_bar.size_x = playerEnt.skill10*5;
}

function enemy_hit(ENTITY *ent)
{
	STRING *deneme = "";

	STRING *bone_name = "deneme";
	ent_bonename(my, bone_name, hit.vertex);
	
	if(playerEnt == you || your.skill11 !=0)
	{
		//do nothing.
	}

	else if (str_stri(bone_name, "Calf") > 0 || str_stri(bone_name, "Foot") > 0)
	{ //FootShot
		my.skill10-= 30;
		my.skill11 =3;
	}
	else if (str_stri(bone_name, "Spine") > 0 || str_stri(bone_name, "Clavicle") > 0)
	{ //BodyShot
		my.skill10-= 50;
		my.skill11 =3;
	}
	else if (str_stri(bone_name, "Neck") > 0 || str_stri(bone_name, "Head") > 0)
	{ //HeadShot
		my.skill10-= 100;
		my.skill11 =3;
	}
	else if (str_stri(bone_name, "Arm") > 0 || str_stri(bone_name, "Shoulder") > 0)
	{ //ArmShot
		my.skill10-= 30;
		my.skill11 =3;
	}
	else
	{ //Registered As Undeclared Limb
		my.skill10-= 30;
		my.skill11 =3;
	}
	
	/*while(1)
	{
		draw_text(str_for_num(NULL, my.skill10),100,60,vector(25,255,25));
		wait(1);
	}*/
	
	if(my.skill10 <= 0)
	{
		
		my.skill11 =2;
		
	}
}
action enemy_loop()
{
	
	my.event = enemy_hit;
	my.emask |= ENABLE_IMPACT;
	
	//#define health skill10
	my.skill10 = 100;
	my.skill11 = 1;

	VECTOR Dir;
	VECTOR MyPos;
	VECTOR TargetPos;
	STRING *state="deneme";

	
	vec_set(Dir,vector(0,0,0));	
	vec_set(MyPos, my.x);
	vec_set(TargetPos, camera.x);
	asFindasPath(MyPos, TargetPos);
	
	var walk_percentage=0;
	var death_percentage=0;
	var hit_percentage=0;
	var attack_percentage=0;
	var attack_timer=0;
	
	
	while (1)
	{
		if(my.skill11 ==1)
		{
		vec_set(MyPos, my.x);
		vec_set(TargetPos, camera.x);
		int i;
		i = c_trace(MyPos, TargetPos, IGNORE_ME | IGNORE_YOU | IGNORE_PASSABLE);
		if (i==0)
		{
			vec_diff(Dir, TargetPos, my.x);

			
			
			if(vec_dist(MyPos, TargetPos)<176)
			{

				my.skill11 = 4;
			}
			
			
		}
		else
		{
			
			
			if (asPath.head == NULL)
			{
				vec_set(MyPos, my.x);
				vec_set(TargetPos, camera.x);
				asFindasPath(MyPos, TargetPos);	

			}
			else
			{

				vec_diff(Dir, asPath.head.pos, my.x);

				// If car near node
				if (vec_dist(my.x, asPath.head.pos) < 100)
				{
					// Make path list iterator pointer points to the first node of the list
					asPath.itr = asPath.head;
					asRemoveNode(asPath); // Remove node that iterator pointer points to	
				}
			}
			asFree();
		}


		vec_to_angle(my.pan, Dir);
		my.tilt = 0;
		vec_normalize(Dir, 15 * time_step);
		c_move(me, nullvector, Dir, IGNORE_PASSABLE | GLIDE);
		
		c_trace(my.x,vector(my.x,my.y,my.z-1000),IGNORE_ME|IGNORE_SPRITES|IGNORE_CONTENT|IGNORE_PASSABLE);
		my.z = hit.z - my.min_z;
		
		
		ent_animate(my, "run", walk_percentage, ANM_CYCLE); 
      walk_percentage += 8 * time_step; 

		}else if(my.skill11 ==2) //death state
		{
			ent_animate(my, "death", death_percentage,ANM_SKIP); 
      	death_percentage += 4*time_step;
		}else if(my.skill11 ==3) //hit state
		{
			ent_animate(my, "hit", hit_percentage,ANM_SKIP); 
      	hit_percentage += 24*time_step;
      	if(hit_percentage >= 100)
      	{
      	hit_percentage =0;
      	my.skill11 =1;	
			}
		}else if(my.skill11 ==4) //attack state
		{
			if(attack_timer > 16)
			{
				ent_animate(my, "attack", attack_percentage,ANM_SKIP); 
	      	attack_percentage += 12*time_step;
	      	if(attack_percentage >= 100)
	      	{
	      	attack_percentage =0;
	      	my.skill11 =5;
	      	attack_timer =0;
	      	player_hit();
				}	
			}else
			{
			my.skill11 =5;	
			}
			
		}else if(my.skill11 ==5) //idle state
		{
			vec_set(MyPos, my.x);
			vec_set(TargetPos, camera.x);
			if(vec_dist(MyPos, TargetPos)>200)
			{
				my.skill11 =1;	
			}else if(attack_timer > 16)
			{
				my.skill11 = 4;
			}else
			{
				ent_animate(my, "idle", walk_percentage,ANM_CYCLE); 
      		walk_percentage += 8*time_step;	
			}
			
		}
		
		attack_timer+=time_step;
		wait(1);
	}
}

function enemy_spawner()
{
	VECTOR spawn_point1;
	vec_set(spawn_point1, vector(4173, 913, 75));

	VECTOR spawn_point2;
	vec_set(spawn_point2, vector(1500, -1300, 75));

	VECTOR spawn_point3;
	vec_set(spawn_point3, vector(100, 975, 75));

	var spawn_index = 0;
	
//	ent_create("Tusker.mdl",spawn_point1,enemy_loop);
	
	while(1)
	{
		if(spawn_index==0)
		{
			ent_create("Tusker.mdl",spawn_point1,enemy_loop);
			spawn_index =1;	
		}else if(spawn_index==1)
		{
			ent_create("Tusker.mdl",spawn_point2,enemy_loop);
			spawn_index =2;	
		}else if(spawn_index==2)
		{
			ent_create("Tusker.mdl",spawn_point3,enemy_loop);
			spawn_index =0;
		}
		
		wait(300);
	}
}

function camera_follow(ENTITY *ent)
{
	while (1)
	{
		vec_set(camera.x, vector(0, 0, 30));				  // camera position relative to the player
		vec_rotate(camera.x, ent.pan);						  // rotate the camera position with the player
		vec_add(camera.x, ent.x);							  // add player position
		vec_set(camera.pan, vector(ent.pan, camera.tilt, 0)); // look in player direction
		camera.tilt = clamp(camera.tilt, -45, 45);
		wait(1);
	}
}

action bullet_fly()
{
	my.ambient = 20;					  // low bright
	my.lightrange = 300;				  // activate dynamic light
	vec_set(my.red, vector(250, 25, 25)); // red light color
	set(me, BRIGHT);					  // additive blending

	vec_scale(my.scale_x, 0.025); // small size
	c_setminmax(me);			  // set my bounding box to my real size
	my.pan = your.pan;			  // face same direction as player
	my.tilt = camera.tilt;
	var isHit = 0;

	while (1)
	{
		// state 1: flying ///////////////////////////////////////////
		if (isHit == 0)
		{
			my.roll += 20 * time_step;
			c_move(me, vector(80 * time_step, 0, 0), NULL, IGNORE_YOU);
			if (HIT_TARGET) // collided?
				isHit = 1;
		}

		// state 2: exploding ////////////////////////////////////////
		if (isHit == 1)
		{
			set(me, ZNEAR); // appear in front of close objects
			my.roll = random(360);
			my.lightrange *= 1 + 0.5 * time_step;		 // increase light range
			vec_scale(my.scale_x, 1 + 0.25 * time_step); // inflate size
			if (my.scale_x > .05)
			{ // explosion finished?
				ent_remove(me);
				return; // terminate function to prevent furter access to removed entity
			}
		}

		wait(1);
	}
}

action Player_movement()
{
	
	camera_follow(me);

	VECTOR vFeet;
	vec_for_min(vFeet, me);

	var attack_timer = 0;
	
	//#define health skill10
	my.skill10 =100;
	my.skill1 =1;
	
	while (1)
	{
		if(my.skill1 ==1)
		{
		
		my.pan -= mouse_force.x * 2;
		camera.tilt += mouse_force.y * 2;

		mouse_pos.x = (screen_size.x - bmap_width(crosshair_pcx)) / 2;
		mouse_pos.y = (screen_size.y - bmap_height(crosshair_pcx)) / 2;

		var distanceX = (key_w - key_s);
		var distanceY = (key_a - key_d);
		VECTOR vc;

		vc.x = distanceX;
		vc.y = distanceY;
		vc.z = 0;

		vec_normalize(vc, 28 * time_step);
		c_move(me, vc, NULL, GLIDE);

		if (mouse_left && attack_timer > 4)
		{
			attack_timer = 0;
			ent_create("bullet.dds", vector(my.x, my.y, my.z + 30), bullet_fly);
		}

		attack_timer += time_step;
	
		
		c_trace(my.x,vector(my.x,my.y,my.z-1000),IGNORE_ME|IGNORE_SPRITES|IGNORE_CONTENT|IGNORE_PASSABLE);
		my.z = hit.z+50;
	}else
	{
		draw_text("You Are Dead",(screen_size.x/2)-56,(screen_size.y/2)-256,vector(25,25,255));
		gameOver_panel.size_x =1000;
		gameOver_panel.size_y = 1000;
	}
		
		wait(1);
	}
}

function main()
{
	video_screen = 1;
	video_mode = 7;
	level_load("littlecity.wmb");
	camera.pan = 0;
	mouse_mode = 1;
	mouse_map = crosshair_pcx;
	enemy_spawner();
	asFree();
	
	VECTOR starting_point;
	vec_set(starting_point, vector(1000, 500, 105));
	playerEnt = ent_create("player.mdl",starting_point,Player_movement);
	
}

