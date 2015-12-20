/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: JODA

/*
To allow the visual studio debugger to display Gameware Navigation container content easily,
add these lines to "Microsoft Visual Studio 9.0\Common7\Packages\Debugger\autoexp.dat"
just before the [hresult] section at the very end of the file.
*/

/*

;------------------------------------------------------------------------------
;  Kaim classes
;------------------------------------------------------------------------------
Kaim::String {
    preview
    (
        #if ( $e.pData != &$e.NullData ) (
            #($e.pData->Data )
         )
         #else (
            #($e.pData->Data, " (Null String)" )
         )
    )
    stringview
    (
        #if ( $e.pData != &$e.NullData.pData ) (
            #($e.pData->Data ) 
         )
         #else (
            #($e.pData->Data, " (Null String)" )
         )
    )
    children
    (
        #(
            #if ( $e.pData != &$e.NullData )
            (
                #(
                    #( Length   : $e.pData->Size ),
                    #( RefCount : $e.pData->RefCount ),
                    #( NullData : $e.NullData )
                )
            )
            #else
            (
                #( RefCount :    $e.pData->RefCount )
            )
        )
    )
}

Kaim::ArrayBase<*>|Kaim::Array<*>|Kaim::ArrayPOD<*>|Kaim::ArrayCPP<*>|Kaim::ArrayLH<*>|Kaim::ArrayLH_POD<*>|Kaim::ArrayDH<*>|Kaim::ArrayDH_POD<*>|Kaim::ArrayCC<*>|Kaim::KyArray<*>|Kaim::KyArrayPOD<*>|Kaim::KyArrayLH<*>|Kaim::KyArrayLH_POD<*>|Kaim::KyArrayDH<*>|Kaim::KyArrayDH_POD<*>{
	preview
	(
		#(
			"[",
			$e.Data.Size,
			"](",
			#array
			(
				expr : ($e.Data.Data)[$i],
				size : $e.Data.Size
			),
			")"
		)
	)
	children
	(
		#(
			[raw members]: [$e,!],
			#array
			(
				expr : ($e.Data.Data)[$i],
				size : $e.Data.Size
			)
		)
	)
}

Kaim::FixedBitSetBase<*>|Kaim::FixedBitSetLH<*>|Kaim::FixedBitSetDH<*>|Kaim::FixedBitSetGH<*> {
	preview
	(
		#(
			"[",
			$e.BitsCount,
			"](",
			#array
			(
				expr : [($e.pData[$i]),x],
				size : ($e.BitsCount + 7) / 8
			),
			")"
		)
	)
	children
	(
		#(
			[raw members]: [$c,!],
			#array
			(
				expr : (bool)(($e.pData[$i >> 3] >> ($i & 0x07)) & 1),
				size : $e.BitsCount
			)
		)
	)
}

Kaim::List<*,*>{
	preview
	(
		#if (&$e.Root == $e.Root.pNext)
		(
			#( "Empty List {Root=", (void*)&$e.Root, "}" )
		)
		#else
		(
			#( "{",
				#list
				(
					head : &$e.Root, ;.pNext,
					next : pNext,
					skip : $e.Root.pPrev
				) : *$e.pNext,
				"}"
			)
		)
	)

	children
	(
		#(
			[raw members]: [$e,!],
			#if (&$e.Root == $e.Root.pNext)
			(
				; Show the root node, just so you can inspect the contents,
				#( Root : $e.Root )
			)
			#else
			(
				#list
				(
					head : &$e.Root,
					next : pNext,
					skip : $e.Root.pPrev
				) : *$e.pNext
			)
		)
	)
}


; Removes the pVoidPrev and pVoidNext from the preview and list, as they are redundant (at least for debugging purposes).
Kaim::ListNode<*> {
	preview
	(
		#( "{pPrev=", &$e.pPrev, " pNext=", &$e.pNext, "}" )
	)

	children
	(
		#(
			pPrev : $e.pPrev,
			pNext : $e.pNext
		)
	)
}

; Displays the RefCount directly in the preview, without the deep inheritance hierarchy.
Kaim::RefCountBase<*>{
    preview
    (
        #( "RefCount=", $e.RefCount )
    )

}


;------------------------------------------------------------------------------
;  Kaim::BitField
;------------------------------------------------------------------------------
Kaim::BitField{
	preview
	(
		#(
			"[",
			$e.m_bitsCount,
			"](",
			#array
			(
				expr : (void*)($e.m_words[$i]),
				size : ($e.m_bitsCount + 31) / 32
			),
			")"
		)
	)
	children
	(
		#(
			[raw members]: [$c,!],
			#array
			(
				expr : (bool)(($e.m_words[$i >> 5] >> ($i & 0x1F)) & 1),
				size : $e.m_bitsCount
			)
		)
	)
}

;------------------------------------------------------------------------------
;  Kaim::BlobRef
;------------------------------------------------------------------------------
Kaim::BlobRef<*>{
	preview
	(
		#(
			expr : (($T1*) &(((char*)&($e.m_offset))[$e.m_offset]) )
		)
	)

	children
	(
		#(
			expr : (($T1*) &(((char*)&($e.m_offset))[$e.m_offset]) ),
			byteSize : $e.m_byteSize
		)
	)
}

;------------------------------------------------------------------------------
;  Kaim::BlobArray
;------------------------------------------------------------------------------
Kaim::BlobArray<*>{
	preview
	(
		#(
			"[",
			$e.m_count,
			"](",
			#array
			(
				expr : (($T1*) &(((char*)&($e.m_offset))[$e.m_offset]) )[$i],
				size : $e.m_count
			),
			")"
		)
	)
	children
	(
		#array
		(
			expr : (($T1*) &(((char*)&($e.m_offset))[$e.m_offset]) )[$i],
			size : $e.m_count
		)
	)
}

;------------------------------------------------------------------------------
;  Kaim::Pool
;------------------------------------------------------------------------------

Kaim::Pool<*>{
	preview
	(
		#(
			"[",
			$e.m_chunkCount,
			"](",
			#array
			(
				expr : $e.m_chunks[$i],
				size : $e.m_chunkCount
			),
			")"
		)
	)
	children
	(
		#(
			[raw members]: [$c,!],
			#array
			(
				expr : $e.m_chunks[$i],
				size : $e.m_chunkCount
			)
		)
	)
}

Kaim::PoolChunk<*>{
	preview
	(
		#(
			"[",
			$e.m_nbSlot - $e.m_nbAvailableSlot,
			"](",
			#array
			(
				expr : $e.m_pool[$i],
				size : $e.m_nbSlot
			),
			")"
		)
	)

	children
	(
		#(
			[raw members]: [$c,!],
			#array
			(
				expr : $e.m_pool[$i],
				size : $e.m_nbSlot
			)
		)
	)
}

Kaim::IterablePool<*>::ConstIterator|Kaim::IterablePool<*>::Iterator{
	preview
	(
		#(
			$e.m_chunkBasedPool->m_chunkBasedPool.m_chunks[$e.m_compactKey.m_chunkIndex]->m_pool[$e.m_compactKey.m_idxInChunk]
		)
	)

	children
	(
		#(
			[raw members] : [$c,!],
			[value] : $e.m_chunkBasedPool->m_chunkBasedPool.m_chunks[$e.m_compactKey.m_chunkIndex]->m_pool[$e.m_compactKey.m_idxInChunk]
		)
	)
}

;------------------------------------------------------------------------------
;  Kaim::CircularArray
;------------------------------------------------------------------------------

Kaim::CircularArray<*>{
	preview
	(
		#(
			"[", $e.m_size, "](",
			#if ($e.m_beginIdx <= $e.m_endIdx)
			(
				#array
				(
					expr : $e.m_buffer[$e.m_beginIdx + $i],
					size : $e.m_size
				)
			)
			#else
			(
				#(
					#array
					(
						expr : $e.m_buffer[$e.m_beginIdx + $i],
						size : $e.m_bufferSize - $e.m_beginIdx
					),
					#if ($e.m_endIdx != 0)
					(
						#(
							", ",
							#array
							(
								expr : $e.m_buffer[$i],
								size : $e.m_endIdx
							)
						)
					)
				)
			),
			")"
		)
	)

	children
	(
		#(
			[raw members]: [$c,!],
			#if ($e.m_beginIdx <= $e.m_endIdx)
			(
				#array
				(
					expr : $e.m_buffer[$e.m_beginIdx + $i],
					size : $e.m_size
				)
			)
			#else
			(
				#(
					#array
					(
						expr : $e.m_buffer[$e.m_beginIdx + $i],
						size : $e.m_bufferSize - $e.m_beginIdx
					),
					#array
					(
						expr : $e.m_buffer[$i],
						size : $e.m_endIdx,
						base : $e.m_bufferSize - $e.m_beginIdx
					)
				)
			)
		)
	)
}

;------------------------------------------------------------------------------
;  Kaim::SharedPoolList
;------------------------------------------------------------------------------

Kaim::SharedPoolList<*>{
	preview
	(
		#if (&$e.m_root == $e.m_root.m_next)
		(
			#( "Empty List {m_root=", (void*)&$e.m_root, "}" )
		)
		#else
		(
			#( "[", $e.m_count, "]{",
				#list
				(
					head : $e.m_root.m_next,
					next : m_next,
					size : $e.m_count,
					skip : &$e.m_root
				) : ($e.m_data),
				"}"
			)
		)
	)

	children
	(
		#(
			[raw members]: [$c,!],
			#if (&$e.m_root == $e.m_root.m_next)
			(
				; Show the root node, just so you can inspect the contents,
				#( m_root : $e.m_root )
			)
			#else
			(
				#list
				(
					head : $e.m_root.m_next,
					next : m_next,
					size : $e.m_count,
					skip : &$e.m_root
				) : ($e.m_data)
			)
		)
	)
}

;------------------------------------------------------------------------------
;  Kaim::Collection
;------------------------------------------------------------------------------

Kaim::Collection<*>|Kaim::TrackedCollection<*>{
	preview
	(
		#(
			"[",
			$e.m_collectionImpl.m_values.Data.Size,
			"](",
			#array
			(
				expr : ($e.m_collectionImpl.m_values.Data.Data)[$i],
				size : $e.m_collectionImpl.m_values.Data.Size
			),
			")"
		)
	)
	children
	(
		#(
			[raw members]: [$e,!],
			#array
			(
				expr : ($e.m_collectionImpl.m_values.Data.Data)[$i],
				size : $e.m_collectionImpl.m_values.Data.Size
			)
		)
	)
}

*/

