# Commands

So to move the object, you have to do it with directionnal keys of the keyboard (up down left right)

move depth with z(brack) and s(front)

# Camera pseudocode

front = normalize( vec3(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch)) )
right = normalize( cross(front, worldUp) )
v = speed * dt

# Map arrows so object appears to move with arrows (move camera opposite)
if arrow_up:    camera.pos -= worldUp * v     # object rises
if arrow_down:  camera.pos += worldUp * v     # object lowers
if arrow_left:  camera.pos += right * v       # object moves left
if arrow_right: camera.pos -= right * v       # object moves right

# Forward/back (conventional)
if W: camera.pos += front * v
if S: camera.pos -= front * v


# triangle roation pseudocode 

# degrees per second
rotSpeed = 90
if Q: triAngleY -= rotSpeed * dt
if E: triAngleY += rotSpeed * dt

model = rotateY( radians(triAngleY) )
mvp   = projection * view(camera) * model
drawTriangle(mvp, color=red)