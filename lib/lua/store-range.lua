local range = redis.call('ZRANGEBYSCORE', KEYS[1], ARGV[1], ARGV[2])

local key = KEYS[2]
local len = #range
for i=1,len do
	redis.call('SADD', key, range[i])
end
return len