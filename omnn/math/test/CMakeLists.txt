test()

target_link_libraries(08_System PUBLIC
	$<$<PLATFORM_ID:Windows>:User32>
)
