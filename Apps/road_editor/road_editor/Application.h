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
#include <map>
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

	enum RotationDirection
	{
		Rotate_CW,
		Rotate_CCW
	};

	struct NodeInformation
	{
		Vector3 location;
		Vector3 direction;
	};

	struct RoadNode
	{
		RoadNode(Vector3& location)
			:	m_location(location)
		{};

		RoadNode()
			:	m_location(Vector3())
		{};

		void SetLocation(const Vector3& newLocation);
		void AddPreviousNode(RoadNode* nodeToAdd);
		void RemoveNextNode(RoadNode* nodeToRemove);
		void AddNextNode(RoadNode* nodeToAdd);
		virtual void Render(MatrixStack& matrixStack, const Vector3& nodeColor = Vector3(1.f, 1.f, 1.f), float sizeMultiplier = 1.f);
		virtual Vector3 GetTangentOfNode();

		Vector3 m_location;
		std::vector< RoadNode* > m_previousNodes;
		std::vector< RoadNode* > m_nextNodes;
	};

	struct RoadNodeCluster
	{
		RoadNodeCluster()
		{};

		void AddNode(RoadNode* nodeToAdd);
		void Render(MatrixStack& matrixStack, const Vector3& nodeColor = Vector3(1.f, 1.f, 1.f), bool renderDirection = false);
		Vector3 GetTangentOfNodeAtSpecifiedIndex(int indexOfRoadNode);
		void InitiateNodeConnections();

		std::vector<RoadNode*> m_roadNodes;
	};

	struct RoadNodeIntersection : public RoadNode
	{
		RoadNodeIntersection(const RoadNode* roadNode)
			:	RoadNode()
		{
			if(roadNode)
			{
				m_previousNodes = roadNode->m_previousNodes;
				m_nextNodes = roadNode->m_nextNodes;
				m_location = roadNode->m_location;

				for(int prevNodeIndex = 0; prevNodeIndex < m_previousNodes.size(); ++prevNodeIndex)
				{
					m_intersectionConnectionsMap[m_previousNodes[prevNodeIndex]] = m_nextNodes;
				}
			}
		};

		virtual void Render(MatrixStack& matrixStack, const Vector3& nodeColor = Vector3(1.f, 1.f, 1.f), float sizeMultiplier = 1.f);
		void AddIncomingRoadNode(RoadNode* incomingRoadNode);
		void AddOutgoingRoadNode(RoadNode* outgoingRoadNode);
		void GetDirectionOfBranchingSegments(std::vector<Vector3>& out_directionSegments);
		bool AllowsForOutgoingNodes();
		Vector3 GetDefaultDirectionOfIntersection();

		std::map< RoadNode*, std::vector< RoadNode* > > m_intersectionConnectionsMap;
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
		void KeyReleased( size_t charReleased );
		void aSyncKeyPressed();
		void readMouseClick(const Vector2& mouseClickLocation);

	private:
		void storeWindowSize( const HWND& windowHandle );
		void render3DScene();
		void render2DScene();
		void CalculateSplineToMousePos();
		int	CalculateIntersectionConnection(const Vector3& mouseWorldPos);
		bool GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(const Vector3& desiredDirection,
			const Matrix4X4& maxRotationMatrix, float maxTurnAngleDotProduct, int indexOfLastValidNode,
			NodeInformation& out_info);
		int AddSemiCircle(int indexOfSemiCircleStart, const Vector3& directionToBuildSemiCircle);
		int AddQuarterCircle(int indexOfLastValidNode, const Vector3& directionToBuild);
		int AddNodesToFaceSpecifiedLocation(const Vector3& locationToFace, const Vector3& startingLocation,
			const Vector3& startingDirection, std::vector< RoadNode* >& nodeContainer, int indexOfNodeToPlace = 0);
		int AddNodesToFaceSpecifiedDirection(const Vector3& directionToFace, const Vector3& startingDirection,
			const Vector3& startingLocation, std::vector< RoadNode* >& nodeContainer, RotationDirection directionToRotate = Rotate_CW,
			int indexOfNodeToPlace = 0);
		RotationDirection GetBestWayToRotateToFaceLocation(const Vector3& startLocation, const Vector3& startDirection,
			const Vector3& endLocation);
		void AddNewTempNode(int indexOfNewNode);
		void AddDebugNodesToIndicateMerge(RoadNode* startNode, RoadNode* nodeToMergeTo);
		void PrecalculateRoadVariables();
		void RenderSplines();
		void ExitSplineMode();
		void DrawHUD();
		void CheckForRoadNodesWithinRange(const Vector3& worldPosition, RoadNodeCluster*& currentRoadNodeCluster,
			RoadNode*& currentRoadNode, int& out_indexOfClosestNode);
		void CheckForIntersectionNodesWithinRange(const Vector3& worldPosition, RoadNodeIntersection*& out_intersectionNode,
			int& out_indexOfIntersectionNode);
		void drawOrigin( float lineLength );
		void initiateDrivingSystem();
		void initiateRoadSystem();
		void updateVehicles( double deltaTime );
		void renderVehicles();
		void UpdateCursorPosition();
		void CalculateFrustumPoints(const Vector3& screenCoordinates, Vector3& nearFrustumWorldCoordinates,
			Vector3& farFrustumWorldCoordinates);
		bool GetMouseWorldPosWithSpecifiedZ(Vector3& out_worldPos, float desiredZValue);
		void RenderDebugNodes();
		void UpdateKeyInput();
		RoadNodeIntersection* ConvertNodeToIntersection(RoadNodeCluster* intersectionRoadNodeCluster, int intersectionNodeIndex);
		bool AddRoad(RoadNodeCluster* roadToAdd);


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
		RoadNodeCluster* m_currentRoadNodeCluster;
		RoadNodeCluster* m_roadNodeClusterInRange;
		RoadNode* m_roadNodeInRange;
		RoadNodeIntersection* m_intersectionNodeInRange;
		int m_intersectionNodeIndex;
		RoadNode* m_forkIntersectionNode;
		RoadNodeCluster* m_roadNodeClusterToSpawnFork;
		RoadNodeIntersection* m_roadNodeIntersectionToForkFrom;
		int m_indexOfForkNode;
		std::vector< RoadNodeCluster* > m_roadNodeClusters;
		std::vector< RoadNodeIntersection* > m_intersectionNodes;
		std::vector< RoadNode* > m_tempNodes;
		std::vector< RoadNode* > m_debugNodesToRender;
		float m_fovy;
		float m_nearZ;
		float m_farZ;
		float m_aspectRatio;
		int m_indexOfLastPermanentNode;
		int m_indexOfLastTempNode;
		float m_lengthOfFragment;
		float m_lengthOfFragmentSquared;
		float m_maxTurnAngleDotProductForRoadSegments;
		float m_maxYawRotationDegreesForRoadSegments;
		Matrix4X4 m_roadMaxCWRotationTransformationMatrix;
		Matrix4X4 m_roadMaxCCWRotationTransformationMatrix;
		float m_HUDFontHeight;
		float m_HUDLineBreakHeight;
		float m_radiusOfRoadCircle;
		bool m_showSecondCurveSystem;
		bool m_showDirectionOnPlacedRoads;
		bool m_showDirectionOnTempNodes;
		bool m_currentRoadIsNew;
	};
	//=================================================================================================
	///////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif