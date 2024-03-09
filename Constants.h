
// Window dimensions upon program launch
#define C_START_WIDTH 1778
#define C_START_HEIGHT 1000

// Camera field of view in radians
#define C_CAMERA_FOV 1.0

// Affects camera movement acceleration, higher means more instantaneous start/stop movement
#define C_KEY_RESPONSIVENESS 4.5

// Controls camera "look" rate
#define C_MOUSE_SENSITIVITY 2.5

// The rate for changing planet size or simulation speed
#define C_MOUSE_SCROLL_SENSITIVITY 1.0

// Camera movement speed, relative to camera height
#define C_CAMERA_SPEED 3.0

// What percent of overall grid radius does it begin to fade out, 1.0 means grid has no fade/alpha
#define C_GRID_FADE_START 0.4

// Major and minor grid line strength/visibility
#define C_GRID_THICK_LINE_ALPHA 0.5
#define C_GRID_THIN_LINE_ALPHA 0.15

// Scales the number of vertices used to render each grid line, higher means smoother gravity distortion
#define C_GRID_RESOLUTION 2000

// Spacing between major grid lines
#define C_GRID_SPACING 1.0

// Number of minor gridlines + 1
#define C_GRID_SUBDIVISIONS 20

// How far can the camera see of the grid
#define C_GRID_RELATIVE_RADIUS 16.0

// Strength of gravity overall
#define C_UNIVERSE_GRAVITY_CONSTANT 500.0

// Change relative mass of bodies
#define C_UNIVERSE_PLANET_GRAVITY 1.0
#define C_UNIVERSE_STAR_GRAVITY 10.0
#define C_UNIVERSE_BLACKHOLE_GRAVITY 10000.0

// Speed at which the user can release celestial bodies
#define C_LAUNCH_SPEED 3.0

// The number of updates per second for celestial bodies; high numbers can cause freezing due to a fixed timestep
#define C_UNIVERSE_UPS 10000.0

#define C_PI2 1.5707963267948966
#define C_PI4 0.7853981633974483