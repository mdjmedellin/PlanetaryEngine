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
		ClickCircle(Vector3& location, float timeUntilExpiration)
			:	m_location(location)
			,	m_expirationTime(timeUntilExpiration)
		{};

		ClickCircle()
			:	m_location(Vector3())
			,	m_expirationTime(1.f)
		{};

		void SetLocation(const Vector3& newLocation);
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
		void UpdateCursorPosition();
		void CalculateFrustumPoints(const Vector3& screenCoordinates, Vector3& nearFrustumWorldCoordinates,
			Vector3& farFrustumWorldCoordinates);
		bool GetMouseWorldPosWithSpecifiedZ(Vector3& out_worldPos, float desiredZValue);

		HWND m_hWnd;
		HDC m_hDC;
		bool m_openCommandConsole, m_displayOrigin;
		MatrixStack m_matrixStack;
		Vector2 m_windowSize;
		Vector3 m_mouseScreenCoordinates;
		Vector3 m_mouseNearFrustumCoordinates;
		Vector3 m_mouseFarFrustumCoordinates;
		Rgba m_backgroundColor;
		RoadSystem* m_roadSystem;
		Camera* m_camera;
		Viewport* m_viewport;
		bool m_splineMode;
		bool m_updateInput;
		bool m_stop;
		VehicleManager* m_vehicleManager;
		std::vector< Vehicle* > m_vehicles;
		std::vector< ClickCircle* > m_tempCircles;
		float m_fovy;
		float m_nearZ;
		float m_farZ;
		float m_aspectRatio;
		int m_indexOfLastPermanentNode;
		int m_indexOfLastTempNode;
		float m_lengthOfFragment;
		float m_lengthOfFragmentSquared;
	};
	//=================================================================================================
	///////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif