// Abmessungen
hoehe    = 20.8;
staerke  = 1;
laenge   = 100;
radius = 2;
loch_d   = 3.5;   // Lochdurchmesser

module e_profil_2d() {
    difference() {

        union() {
            // Langer Schenkel / Rücken
            translate([-1,0])         
            square([2, hoehe]);
            
             hull() {
                translate([-1, radius])
                    circle(r = radius, $fn = 64);

                translate([-1, hoehe - radius])
                    circle(r = radius, $fn = 64);
            }
            

            // Unterer Schenkel
            square([4, 1]);
            
            translate([0, 2.4])
            square([4, 1]); 
            
            // Mittlerer Klotz
            translate([0, 5.3])
            square([6, 9.6]);            
            
 
            translate([0, 16.8])
            square([4, 1.6]); 
            
            // oberer Schenkel
            translate([0, hoehe - 1])
            square([4, 1]);

        }

        // Loch mittig im langen 5-mm-Schenkel
        translate([
            2,
            hoehe / 2
        ])
            circle(d = loch_d, $fn = 64);
    }
}

// E-Kontur inklusive Loch 150 mm extrudieren
linear_extrude(height = laenge)
    e_profil_2d();