print("Hello from Lua")

function move_box()
    local x = GetBoxX()
    local y = GetBoxY()
    Sol2_MoveBox(x + 1, y + 1)
end

move_box()