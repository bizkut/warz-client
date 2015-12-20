#ifndef HOFFMANSCATTER_H_INCLUDED
#define HOFFMANSCATTER_H_INCLUDED

//-----------------------------------------------------------------------------
//  HoffmanScatter
//-----------------------------------------------------------------------------
//
//! Implements the scattering model as described by Hoffman et al.
//
//-----------------------------------------------------------------------------
class HoffmanScatter
{
public:
    HoffmanScatter();
    ~HoffmanScatter();
    void create();
     
    //
    // Accessors
    //

    //! \brief  Sets the current sun intensity
    //! \param  [in] The sun intensity to set
    void            setSunIntensity         ( const float a_value );
    //! \brief  Sets the current Mie scattering coefficient
    //! \param  [in] The Mie scattering coefficient to set
    void            setMie                  ( const float a_value );
    //! \brief  Sets the current Rayleigh scattering coefficient
    //! \param  [in] The Rayleigh scattering coefficient to set
    void            setRayleigh             ( const float a_value );
    //! \brief  Sets the current Henyey Greenstein g-value
    //! \param  [in] The Henyey Greenstein g-value to set
    void            setHg                   ( const float a_value );
    //! \brief  Sets the current inscattering factor
    //! \param  [in] The inscattering factor to set
    void            setInscattering         ( const float a_value );
    //! \brief  Sets the current turbiditiy
    //! \param  [in] The turbiditiy to set
    void            setTurbidity            ( const float a_value );

    //
    // Interface IScatter implementation
    //

    //! \brief  Returns whether the skylight renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    const bool      isSkyLightValid         () const;


    //! \brief  Returns whether the aerial perspective renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    const bool      isAerialPerspectiveValid() const;

    //! \brief  Renders skydome and skylight
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The sky dome vertices
    //! \param  [in] The sky dome indices
    //! \param  [in] The sky dome index count
    void            renderSkyLight          ( const r3dCamera& a_camera
                                            , const r3dPoint3D& a_sunDirection
                                            , const r3dPoint3D& tFog_CloudAnim_Params
                                            , r3dColor fogColor
                                            , r3dPoint2D tCloudsDim
                                            , bool normals
                                            , float mie
											, float angle
											, float angle_range
											, bool setShaders
                                            );

    //! \brief  Renders terrain and aerial perspective
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The terrain vertices
    //! \param  [in] The terrain texture coordinates
    //! \param  [in] The terrain indices
    //! \param  [in] The terrain index count
    //! \param  [in] The terrain texture map
    //! \param  [in] The terrain normal map
    void            renderAerialPerspective ( const r3dCamera& a_camera
                                            , const r3dPoint3D& a_sunDirection
											, const r3dPoint3D& tFog_CloudAnim_Params
                                            , r3dColor fogColor
											, r3dPoint2D tCloudsDim
											, float mie
											, float angle
											, float angle_range
											);

public:

    //
    // Construction and assignment
    //

    //! \brief  Creates a HoffmanScatter from the specified one
    //! \param  [in] The HoffmanScatter to copy from
    HoffmanScatter          ( const HoffmanScatter& );
    //! \brief  Sets this HoffmanScatter to the specified one
    //! \param  [in] The HoffmanScatter to copy from
    //! \return The modified HoffmanScatter
    HoffmanScatter& operator=               ( const HoffmanScatter& );

    //
    // Helper
    //

    //! \brief  Computes the sun attenuation for the specified sun angle
    //! \param  [in] The sun angle to use
    void            computeAttenuation      ( const float a_theta, float angle_range );
    //! \brief  Updates the shader constants
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    void            setShaderConstants      ( const r3dCamera& a_camera
                                            , const r3dPoint3D& a_sunDirection
											, const r3dPoint3D& tFog_CloudAnim_Params
											, r3dColor fogColor
											, r3dPoint2D tCloudsDim
											, float mieScale
											, float amplify
                                            );

    //
    // Atmosphere data
    //

    float       m_HGg;                      //!< g value in Henyey Greenstein approximation function
    float       m_inscatteringMultiplier;   //!< Multiply inscattering term with this factor
    float       m_betaRayMultiplier;        //!< Multiply Rayleigh scattering coefficient with this factor
    float       m_betaMieMultiplier;        //!< Multiply Mie scattering coefficient with this factor
    float       m_sunIntensity;             //!< Current sun intensity
    float       m_turbitity;                //!< Current turbidity
    float       m_fCloudsScale;
    float       m_fCloudDensity;

	float		m_fSkySunsetBumpness;
	float		m_fSkySunsetOffset;
	float		m_fSkySunsetPower;
	float		m_fSkySunsetStartCoef;
	float		m_fSkyCloudsFadeStart;
	float		m_fSkyCloudsFadeEnd;

	float		m_fSkyIntensity;

    r3dPoint3D    m_betaRay;                  //!< Rayleigh total scattering coefficient
    r3dPoint3D    m_betaDashRay;              //!< Rayleigh angular scattering coefficient without phase term
    r3dPoint3D    m_betaMie;                  //!< Mie total scattering coefficient
    r3dPoint3D    m_betaDashMie;              //!< Mie angular scattering coefficient without phase term
    D3DXVECTOR4	  m_sunColorAndIntensity;     //!< Sun color and intensity packed for the shader

    //
    // Cg data
    //
    int		m_cgVertexShader;           //!< Shared Hoffman vertex shader
    int		m_cgSkyFragmentShader;      //!< Sky fragment shader
    int		m_cgSkyFragmentShaderLQ;      //!< Sky fragment shader

	int		m_cgSkyFragmentShader_Normals;      //!< Sky fragment shader
	int		m_cgSkyFragmentShader_NormalsLQ;    //!< Sky fragment shader


    enum VSConst
    {
      // vertex shader constants
      VSC_EyePos = 20,                  //!< Shared vertex shader eye position parameter
      VSC_SunDir = 21,                  //!< Shared vertex shader sun direction parameter
      VSC_BetaRPlusBetaM = 22,          //!< Shared vertex shader scattering coefficients parameter
      VSC_HGg = 23,                     //!< Shared vertex shader Henyey Greenstein parameter
      VSC_BetaDashR = 24,               //!< Shared vertex shader scattering coefficients parameter
      VSC_BetaDashM = 25,               //!< Shared vertex shader scattering coefficients parameter
      VSC_OneOverBetaRPlusBetaM = 26,   //!< Shared vertex shader scattering coefficients parameter
      VSC_Multipliers = 27,             //!< Shared vertex shader multiplier parameter
      VSC_SunColorAndIntensity = 28,    //!< Shared vertex shader sun color/intensity parameter
      VSC_FogColor = 29,                //!< Shared vertex shader fog color
      VSC_SunsetParams = 30,            //!<
      VSC_SunsetCloudsParams = 31,      //!<
      VSC_FogParams = 32,                //!< Shared vertex shader fog color
      VSC_MVP = 0,                      //!< Shared vertex shader model/view/projection matrix parameter
      VSC_MV  = 4,                      //!< Shared vertex shader model/view matrix parameter
    };

    enum PSConst
    {
      PSC_EyePos = 0,                   //!< Shared vertex shader eye position parameter
      PSC_SunDir = 1,                   //!< Shared vertex shader sun direction parameter
      PSC_BetaRPlusBetaM = 2,           //!< Shared vertex shader scattering coefficients parameter
      PSC_HGg = 3,                      //!< Shared vertex shader Henyey Greenstein parameter
      PSC_BetaDashR = 4,                //!< Shared vertex shader scattering coefficients parameter
      PSC_BetaDashM = 5,                //!< Shared vertex shader scattering coefficients parameter
      PSC_OneOverBetaRPlusBetaM = 6,    //!< Shared vertex shader scattering coefficients parameter
      PSC_Multipliers = 7,              //!< Shared vertex shader multiplier parameter
      PSC_SunColorAndIntensity = 8,     //!< Shared vertex shader sun color/intensity parameter
      PSC_SunsetParams = 9,             //!<
      PSC_SunsetCloudsParams = 10,      //!<
	  PSC_MieScale = 11,                //!<
	  PSC_Amplify = 12,                //!<
      PSC_MVP = 0,                      //!< Shared vertex shader model/view/projection matrix parameter
      PSC_MV  = 4,                      //!< Shared vertex shader model/view matrix parameter

      // pixesl shader constants
      PSC_LightVector = 0,            //!< Terrain fragment shader light vector parameter
      // pixel shader textures
      PSC_TerrainMap  = 0,             //!< Terrain fragment shader texture map parameter
      PSC_NormalMap   = 1,              //!< Terrain fragment shader normal map parameter
    };

    bool        m_validSkyLight;            //!< Flag indicating whether the skylight shader is loadable
    bool        m_validAerialPerspective;   //!< Flag indicating whether the airlight shader is loadable

};


extern   HoffmanScatter hoffman;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // HOFFMANSCATTER_H_INCLUDED
