// Unité: mm
R = 1000/3.141592/2;
fn = 300;
sf = 40;

//color([0,1,0])cube([318,20,20], center=true);

projection(cut = true)
  translate([0,0,-0]) // -4 pour dessus, 0 dessous
  horloge();

module horloge(){
// Rond externe
 translate([0,0,-7])
 difference(){
  rond();
  color([0.9,0,0])cylinder(h=13, r=R, $fn=fn);
  rotate(a=30)translate([0,0,5])color([0,0,1])cube([900,0.1,10], center=true);
  rotate(a=30+90)translate([0,0,5])cube([900,0.1,10], center=true);
 }

 // lettres
 add("3", 0.95, 0);
 add("6", 0, -0.9);
 add("9", -0.95, 0);
 add("12", 0, 0.9);
}

module rond(){
 cylinder(h=13, r=R+10, $fn=fn); // 1cm d'épaisseur
 // base
 translate([0,-R-4,8.5])
 scale([3,3,1])difference(){
  translate([  0,0,-2]) cube([30,6,13], center=true);
  translate([+20,0,-10]) cylinder(h=20, r=6, $fn=fn);
  translate([-20,0,-10]) cylinder(h=20, r=6, $fn=fn);
  }
}

module add(txt, dx, dy) {
  translate([R*dx, R*dy, 3])
  linear_extrude(height=3, convexity=4)
  text(txt, 
    size=sf,
    font="Jokerman",
    halign="center",
    valign="center", $fn=fn);
}