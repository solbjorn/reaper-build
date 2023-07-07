function normal		(shader, t_base, t_second, t_detail)
	shader:begin		("dumb","dumb")
			: fog		(false)
			: zb 		(false,false)
			: blend		(true,blend.srcalpha,blend.invsrcalpha)
end
