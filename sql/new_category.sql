insert into category ( rid_parent, icon, title, registered_at,
					   fast, stat_code, locked_at, acomment   )
	values( :rid_parent, :icon, :title, :registered_at,
            :fast, :stat_code, :locked_at, :acomment    )
	returning iid
