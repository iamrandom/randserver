

-- print(package.path)
package.path = package.path .. ";../../luacode/?.lua"

require("serialize")
-- require("common.log")


local ser = serialize.new_serialize(0, 1024)
local deser = serialize.new_deserialize()


print(ser:pack(1, 2, 3))
print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXX")
print(deser:unpack(ser))
-- print(ser:pack("1dfsfds", a))
-- print(deser:unpack(ser))
-- print(deser:unpack(ser))
print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXX")
print(123)

-- old_print(123)



