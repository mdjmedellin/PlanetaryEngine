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
		Rotate_CCW,
		Rotate_NONE,
		Rotate_COUNT
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
		{
			for(int i = 0; i < Rotate_COUNT; ++i)
			{
				m_nextNodesDirection[i] = nullptr;
				m_previousNodesDirection[i] = nullptr;
			}
		};

		RoadNode()
			:	m_location(Vector3())
		{
			for(int i = 0; i < Rotate_COUNT; ++i)
			{
				m_nextNodesDirection[i] = nullptr;
				m_previousNodesDirection[i] = nullptr;
			}
		};

		RoadNode( const RoadNode* nodeToCopy )
		{
			if(nodeToCopy != nullptr)
			{
				for(int i = 0; i < Rotate_COUNT; ++i)
				{
					m_nextNodesDirection[i] = nodeToCopy->m_nextNodesDirection[i];
					m_previousNodesDirection[i] = nodeToCopy->m_previousNodesDirection[i];
				}
			}

			m_nextNodes = nodeToCopy->m_nextNodes;
			m_previousNodes = nodeToCopy->m_previousNodes;
			m_isValid = nodeToCopy->m_isValid;
			m_location = nodeToCopy->m_location;
		}

		void SetLocation(const Vector3& newLocation);
		void AddPreviousNode(RoadNode* nodeToAdd);
		void RemoveNextNode(RoadNode* nodeToRemove);
		void AddNextNode(RoadNode* nodeToAdd, bool isPermanentlyAdded = false);
		virtual void Render(MatrixStack& matrixStack, const Vector3& nodeColor = Vector3(1.f, 1.f, 1.f), float sizeMultiplier = 1.f);
		virtual Vector3 GetTangentOfNode();
		virtual RotationDirection GetBestPossibleDirectionToBranch( const Vector3& goalLocation, const Matrix4X4& maxCWTransformationMatrix,
			const Matrix4X4& maxCCWTransformationMatrix );
		virtual RotationDirection GetBestPossibleDirectionToMerge( const Vector3& goalLocation, const Matrix4X4& maxCWTransformationMatrix,
			const Matrix4X4& maxCCWTransformationMatrix );
		void ReplaceNextNodeWithSpecifiedNode(RoadNode* nodeToRemove, RoadNode* nodeToAdd);
		void ReplacePreviousNodeWithSpecifiedNode(RoadNode* nodeToRemove, RoadNode* nodeToAdd);
		Vector3 m_location;
		std::vector< RoadNode* > m_previousNodes;
		std::vector< RoadNode* > m_nextNodes;
		bool m_isValid;
		RoadNode* m_nextNodesDirection[Rotate_COUNT];
		RoadNode* m_previousNodesDirection[Rotate_COUNT];
	};

	struct RoadNodeCluster
	{
		RoadNodeCluster()
		{};

		void AddNode(RoadNode* nodeToAdd);
		void Render(MatrixStack& matrixStack, float scale = 1.f, const Vector3& nodeColor = Vector3(1.f, 1.f, 1.f), bool renderDirection = false);
		Vector3 GetTangentOfNodeAtSpecifiedIndex(int indexOfRoadNode);
		void InitiateNodeConnections(bool isPermanentlyAdded = false);

		std::vector<RoadNode*> m_roadNodes;
	};

	struct RoadNodeIntersection : public RoadNode
	{
		RoadNodeIntersection(const RoadNode* roadNode)
			:	RoadNode( roadNode )
		{
			for(int prevNodeIndex = 0; prevNodeIndex < m_previousNodes.size(); ++prevNodeIndex)
			{
				m_intersectionConnectionsMap[m_previousNodes[prevNodeIndex]] = m_nextNodes;
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
		void testFunctionName();
		int AddNodesToGetAsCloseAsPossibleToGoal(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode);
		int	CalculateIntersectionConnection(const Vector3& mouseWorldPos);
		int ConstructBestPossibleRoad(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode);
		int ConstructRoadFromNodeWithIndex0(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode);
		int ConstructRoadTowardsSpecifiedLocation(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode);
		int ConstructTurnTowardsSpecifiedLocation(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode);
		int CreateDirectConnection(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode);
		int BranchRoadFromSpecifiedNodeTowardsGoalLocation(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode);
		bool GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(const Vector3& desiredDirection,
			const Matrix4X4& maxRotationMatrix, float maxTurnAngleDotProduct, int indexOfLastValidNode,
			NodeInformation& out_info);
		int AddSemiCircle(int indexOfSemiCircleStart, const Vector3& directionOfStartNode, const Vector3& goalLocation);
		int AddQuarterCircle(int indexOfQuarterCircleStart, const Vector3& directionOfStartNode, const Vector3& goalLocation);
		int AddNodesToFaceSpecifiedLocation(const Vector3& locationToFace, const Vector3& startingLocation,
			const Vector3& startingDirection, std::vector< RoadNode* >& nodeContainer, int indexOfNodeToPlace = 0);
		int AddNodesToFaceSpecifiedDirection(const Vector3& directionToFace, const Vector3& startingDirection,
			const Vector3& startingLocation, std::vector< RoadNode* >& nodeContainer, RotationDirection directionToRotate = Rotate_CW,
			int indexOfNodeToPlace = 0);
		RotationDirection GetBestWayToRotateToFaceLocation(const Vector3& startLocation, const Vector3& startDirection,
			const Vector3& endLocation);
		RotationDirection GetBestSuitedRotationMatrix(const Vector3& startLocation, const Vector3& startDirection, const Vector3& endLocation,
			Matrix4X4& out_bestSuitedRotationMatrix);
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
		void AddCurrentRoadToIntersection(RoadNode* intersection);


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
		RoadNodeIntersection* m_intersectionNodeToForkFrom;
		RoadNodeCluster* m_roadNodeClusterToSpawnFork;
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
		Matrix4X4 m_rotationMatrix90DegreesCW;
		float m_HUDFontHeight;
		float m_HUDLineBreakHeight;
		float m_radiusOfRoadCircle;
		bool m_showSecondCurveSystem;
		bool m_showDirectionOnPlacedRoads;
		bool m_showDirectionOnTempNodes;
		bool m_currentRoadIsNew;
		float m_originalZVal;
		float m_scale;
	};
	//=================================================================================================
	///////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif