/****************************************************************************

Routine:	       	imgcoord.c

Author/Copyright:      	Hans-Gerd Maas

Address:	       	Institute of Geodesy and Photogrammetry
                        ETH - Hoenggerberg
		       	CH - 8093 Zurich

Creation Date:	       	22.4.88

Description:	       	computes x', y' from given Point and orientation
	       		(see: Kraus)

Routines contained:

****************************************************************************/

#include "imgcoord.h"


/* flat_image_coord() calculates projection from coordinates in
    world space to metric coordinates in image space without 
    distortions
    Arguments:
    doubles X,Y,Z in real space
    Calibration *cal parameters pointer
    multimedia *mm parameters pointer
    int i_cam - camera number (from 0 to cpar->num_cams)
    multimedia look-up table array mmLUT pointer
    Output:
    double x,y in pixel coordinates in the image space
 */
 
void flat_image_coord (vec3d pos, Calibration *cal, mm_np *mm, double *x, double *y){

  double deno;
  Calibration cal_t;
  double X_t,Y_t,Z_t,cross_p[3],cross_c[3];
  vec3d pos_t;
  
  
  cal_t.mmlut = cal->mmlut;

  /* calculate tilted positions and copy them to X_t, Y_t and Z_t */
  
    trans_Cam_Point(cal->ext_par, *mm, cal->glass_par, pos, \
         &(cal_t.ext_par), pos_t, cross_p, cross_c);
    
    multimed_nlay (&cal_t, mm, pos_t, &X_t,&Y_t);
    
    vec_set(pos_t,X_t,Y_t,pos_t[2]);
    
    back_trans_Point(pos_t, *mm, cal->glass_par, cross_p, cross_c, pos);
	  

    deno = cal->ext_par.dm[0][2] * (pos[0]-cal->ext_par.x0)
    + cal->ext_par.dm[1][2] * (pos[1]-cal->ext_par.y0)
    + cal->ext_par.dm[2][2] * (pos[2]-cal->ext_par.z0);

    *x = - cal->int_par.cc *  (cal->ext_par.dm[0][0] * (pos[0]-cal->ext_par.x0)
          + cal->ext_par.dm[1][0] * (pos[1]-cal->ext_par.y0)
          + cal->ext_par.dm[2][0] * (pos[2]-cal->ext_par.z0)) / deno;

    *y = - cal->int_par.cc *  (cal->ext_par.dm[0][1] * (pos[0]-cal->ext_par.x0)
          + cal->ext_par.dm[1][1] * (pos[1]-cal->ext_par.y0)
          + cal->ext_par.dm[2][1] * (pos[2]-cal->ext_par.z0)) / deno;
}

/*  img_coord() uses flat_image_coord() to estimate metric coordinates in image space
    from the 3D position in the world and distorts it using the 
    Brown distortion model https://en.wikipedia.org/wiki/Distortion_(optics)
    
    Arguments:
    vec3d pos is a vector of position in 3D (X,Y,Z real space)
    Calibration *cal parameters pointer of a specific camera
    multimedia *mm parameters pointer
    int i_cam - camera number (from 0 to cpar->num_cams)
    multimedia look-up table array mmLUT pointer
    Output:
    double x,y in pixel coordinates in the image space
 */
void img_coord (vec3d pos, Calibration *cal, mm_np *mm, double *x, double *y){
    	
    flat_image_coord (pos, cal, mm, x, y);

    distort_brown_affin (*x, *y, cal->added_par, x, y);

}


