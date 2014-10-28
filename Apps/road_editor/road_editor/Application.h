#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

////////////////////////////////////////////////////////////////////////////////////////////
//=========================================INCLUDES=========================================
#include "MatrixStack.h"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.h"
#include "Rgba.h"
//==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////

namespace gh
{
	//////////////////////////////////////////////////////////////////////////////////
	//=================================== FORWARD DECLARATIONS =======================
	class RoadSystem;
	class Vehicle;
	class Camera;
	class Intersection;
	class VehicleManager;
	class Viewport;
	//================================================================================
	//////////////////////////////////////////////////////////////////////////////////


	struct ClickCircle
	{
		ClickCircle(Vector4& location, float timeUntilExpiration)
		{
			m_location.setXYZ(location.x, location.y, location.z);
			m_expirationTime = timeUntilExpiration;
		}

		void Render(MatrixStack& matrixStack);

		Vector3 m_location;
		float m_expirationTime;
	};



	//////////////////////////////////////////////////////////////////////////////////
	//=============================APPLICATION========================================
	class Application
	{
	public:
		Application( HWND handleWnd, HDC handleDC );
		~Application();
		void onResize( int width, int height );
		void updateFrame( double deltaTime );
		void toggleCommandConsole();
		void toggleOrigin();
		void updateInput( bool continueUpdating );
		void keyPressed( size_t charPressed );
		void aSyncKeyPressed();
		void readMouseClick(const Vector2& mouseClickLocation);

	private:
		void storeWindowSize( const HWND& windowHandle );
		void render3DScene();
		void render2DScene();
		void drawOrigin( float lineLength );
		void initiateDrivingSystem();
		void initiateRoadSystem();
		void updateVehicles( double deltaTime );
		void renderVehicles();

		HWND m_hWnd;
		HDC m_hDC;
		bool m_openCommandConsole, m_displayOrigin;
		MatrixStack m_matrixStack;
		Vector2 m_windowSize;
		Rgba m_backgroundColor;
		RoadSystem* m_roadSystem;
		Camera* m_camera;
		Viewport* m_viewport;
		bool m_updateInput;
		bool m_stop;
		VehicleManager* m_vehicleManager;
		std::vector< Vehicle* > m_vehicles;
		std::vector< ClickCircle* > m_tempCircles;
		float m_fovy;
		float m_nearZ;
		float m_farZ;
	};
	//=================================================================================================
	///////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif