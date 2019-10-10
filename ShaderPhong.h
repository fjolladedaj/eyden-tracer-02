#pragma once

#include "ShaderFlat.h"
using namespace std;

class CScene;

class CShaderPhong : public CShaderFlat
{
public:
	/**
	 * @brief Constructor
	 * @param scene Reference to the scene
	 * @param color The color of the object
	 * @param ka The ambient coefficient
	 * @param kd The diffuse reflection coefficients
	 * @param ks The specular refelection coefficients
	 * @param ke The shininess exponent
	 */
	CShaderPhong(CScene &scene, Vec3f color, float ka, float kd, float ks, float ke)
		: CShaderFlat(color), m_scene(scene), m_ka(ka), m_kd(kd), m_ks(ks), m_ke(ke)
	{
	}
	virtual ~CShaderPhong(void) = default;

	virtual Vec3f Shade(const Ray &ray) const override
	{
		// --- PUT YOUR CODE HERE ---
		Vec3f Cd;
		Vec3f Ca;
		Vec3f Cs;
		Vec3f difference = 0;
		Vec3f spec = 0;
		int m = m_scene.m_vpLights.size();
		Ray l, t;

		for (int i = 0; i < m; i++)
		{
			l.org = ray.org + ray.t * ray.dir;
			l.t = numeric_limits<float>::infinity();
			optional<Vec3f> lightRadiance = m_scene.m_vpLights[i]->Illuminate(l);
			if (!m_scene.Occluded(l))
			{
				if (lightRadiance)
				{
					float theta = max(l.dir.dot(ray.hit->GetNormal(ray)), 0.0f);
					difference += *lightRadiance * theta;
				}
			}
		}

		for (int i = 0; i < m; i++)
		{
			t.org = ray.org + ray.t * ray.dir;
			t.t = numeric_limits<float>::infinity();
			optional<Vec3f> lightRadiance = m_scene.m_vpLights[i]->Illuminate(t);
			if (!m_scene.Occluded(l))
			{
				if (lightRadiance)
				{
					Vec3f reflectedDir = t.dir - 2 * (t.dir.dot(ray.hit->GetNormal(ray))) * ray.hit->GetNormal(ray);
					float theta = max(ray.dir.dot(reflectedDir), 0.0f);
					spec = spec + (*lightRadiance * pow(theta, m_ke));
				}
			}
		}

		Ca = m_ka * CShaderFlat::Shade(ray);
		Cd = m_kd * CShaderFlat::Shade(ray).mul(difference);
		Cs = m_ks * RGB(1, 1, 1).mul(spec);
		return Ca + Cd + Cs;
	}

private:
	CScene &m_scene;
	float m_ka; ///< ambient coefficient
	float m_kd; ///< diffuse reflection coefficients
	float m_ks; ///< specular refelection coefficients
	float m_ke; ///< shininess exponent
};