/* [Basic] */
//Knob Diameter (mm)
knob_diameter=25; //[10:30]
//Knob Height (mm)
knob_height=15;//[10:20]
//Shaft Length (mm)
shaft_length=10;//[5:15]

/* [Customizations] */
//Finger Indent (mm)
Finger_Indent=5;//[1:10]
//Roundover (mm)
roundover=2;//[1:3]
//tolerance (mm)
tolerance=0.1;//[0,0.1,0.2,0.3]
//Side Detent Depth (mm)
detent=0.7;//[0.5:0.1:2.0]
//Detent Angle Spacing (degrees)
detent_angle=30;//[15:15:90]

/* [Hidden] */
$fn=50;

difference() {
    // Knob Body
  union() {
      cylinder(r=knob_diameter/2,h=knob_height-roundover);
      translate([0,0,knob_height-roundover]) cylinder(r=knob_diameter/2-roundover,h=roundover);
      translate([0,0,knob_height-roundover]) rotate_extrude(convexity = 10) translate([knob_diameter/2-roundover,0,0])  circle(r=roundover);
  }
  // Finger Detent
  translate([knob_diameter/2-Finger_Indent-roundover*2,knob_diameter/2-Finger_Indent-roundover,knob_height+Finger_Indent/1.5]) sphere(r=Finger_Indent);
  
  // Side Detents
  for (i=[0:detent_angle:360]) translate([(knob_diameter+0.6)*sin(i)/2,(knob_diameter+0.6)*cos(i)/2,3]) cylinder(r=detent,h=knob_height);
  difference() {
      translate([0,0,2]) cylinder(r=3.1+tolerance,h=shaft_length-2);
      translate([-5,-6.6-tolerance,2]) cube([10,5,shaft_length-2]);
  }
  
  // Shaft Hole
  difference() {
      cylinder(r=3+tolerance,h=2);
      translate([-5,-6.5-tolerance,0]) cube([10,5,shaft_length-2]);
  }
}
