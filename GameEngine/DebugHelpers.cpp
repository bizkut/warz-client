#include "r3dPCH.h"

#pragma warning( disable : 4244 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4800 )

#include "r3d.h"

#include "GameCommon.h"

static void PrintPos( const D3DXMATRIX& vp, const r3dPoint3D& pos, const char* str )
{
	D3DXVECTOR4 screenVec( pos.x, pos.y, pos.z, 1.f );

	D3DXVec4Transform( &screenVec, &screenVec, &vp );

	screenVec.x /= screenVec.w;
	screenVec.y /= screenVec.w;

	float w = r3dRenderer->ScreenW ;
	float h = r3dRenderer->ScreenH ;

	screenVec.x = screenVec.x * 0.5f + 0.5f;
	screenVec.y = - screenVec.y * 0.5f + 0.5f;

	MenuFont_Editor->PrintF( screenVec.x * w, screenVec.y * h, r3dColor24( 0xff80ff50 ), str );
}


void DrawDebugPositions( const Positions& poses )
{
	D3DXMATRIX vp = r3dRenderer->ViewProjMatrix;

	char num[ 16 ] = { 0 };

	for( uint32_t i = 0, e = poses.Count(); i < e; i ++ )
	{
		_snprintf( num, sizeof num - 1, "%d", i );

		PrintPos( vp, poses[ i ], num );
	}
}


float elapsed;

void DrawOverlappingPositions( const Positions& poses )
{
	if( poses.Count() )
	{
		D3DXMATRIX vp = r3dRenderer->ViewProjMatrix;

		typedef r3dTL::TFixedAllocator< std::pair< const UINT64, const r3dPoint3D* > > Alloc;
		typedef std::multimap< UINT64, const r3dPoint3D*, std::less<UINT64>, Alloc > PosMap;

		PosMap posMap( PosMap::key_compare(), Alloc( poses.Count() * 2 ) );

		float ftime = r3dGetTime();

		for( uint32_t i = 0, e = poses.Count(); i < e; i ++ )
		{
			const r3dPoint3D& pos = poses[ i ] ;
			UINT64 key = (UINT64)(UINT32&)poses[ i ].X + ((UINT64)(UINT32&)poses[ i ].Z << 32ull );

			posMap.insert( PosMap::value_type( key, &pos ) );
		}

		elapsed = r3dGetTime() - ftime ;

		PosMap::const_iterator prevIter = posMap.begin() ;

		char str[ 512 ];
		char num[ 16 ] = { 0 };

		const r3dPoint3D* start = &poses[ 0 ];

		r3dTL::TArray< int > Visited;

		Visited.Resize( 512 );

		int count = 0;

		for( PosMap::const_iterator i = posMap.begin(), e = posMap.end(); i != e; )
		{
			++ i;
			count ++ ;

			if( i == e || i->first != prevIter->first )
			{
				Visited.Clear();
				Visited.Resize( count, 0 );

				PosMap::const_iterator last = i;
				--last;

				int output_count = 0;

				do
				{
					str[ 0 ] = 0;
					int totalLen = 0;

					int full = 0;
					int vi = 0;

					PosMap::const_iterator ii = prevIter ;

					while( Visited[vi] && vi < count )
					{
						vi ++;
						++ ii;
					}

					if( vi == count )
						break ;

					r3dPoint3D pos = *ii->second;

					UINT32 y = (UINT32&)pos.y;

					for( ; ii != i ; ++ ii, vi ++ )
					{
						if( !Visited[ vi ] && (UINT32&)ii->second->y == y )
						{
							_snprintf( num, sizeof num - 1, "%d,", (int)( ii->second - start ) );

							if( totalLen < 48 && !full )
							{
								strcat ( str, num ) ;
								totalLen += strlen( num ) ;
							}
							else
							{
								full = 1;
								// kill last comma
								strcat( str - 1, "..." );
								totalLen += 3 ;
							}

							output_count ++ ;

							Visited[ vi ] = 1 ;
						}
					}

					if( totalLen )
					{
						// kill last comma
						str[ strlen(str) - 1 ] = 0;
						PrintPos( vp, pos, str );
					}
				}
				while( output_count != count );

				prevIter = i ;
				count = 0 ;
			}
		}
	}
}
