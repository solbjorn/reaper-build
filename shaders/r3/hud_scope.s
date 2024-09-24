function normal(shader, t_base, t_second, t_detail)
	shader:begin("hud_scope", "hud_scope")
		: sorting(1, false)
		: blend  (true, blend.srcalpha, blend.invsrcalpha)
		: aref   (false, 0)
		: zb     (false, false)
		: distort(false)
		: fog    (false)
	shader:dx10texture("s_base", t_base)
	shader:dx10sampler("smp_base")
end
