#ifndef __ETERNITYEXP__H
#define __ETERNITYEXP__H

class CMesh;

// This is the main class for the exporter.
class EternityExp
{
 public:
	EternityExp();
	~EternityExp();

	void		ExportGeomObjectAscii(FILE *fMesh, const CMesh* mesh);


	// Misc methods
	void		CommaScan(TCHAR* buf);

	// A collection of overloaded value to string converters.
	CStr		Format(int value);
	CStr		Format(float value);
	CStr		Format(Point3 value, bool convert = true);
	CStr		FormatColor(VertColor value);
	CStr		FormatUV(UVVert value, bool convert = true);
	CStr		Format(Color value);
	CStr		Format(AngAxis value);
	CStr		Format(Quat value);

	int		nUVPrecision;
	int		nGeometryPrecision;
	TCHAR		szFmtUVStr[16];
	TCHAR		szFmtGeometryStr[16];
};

#endif // __ETERNITYEXP__H

