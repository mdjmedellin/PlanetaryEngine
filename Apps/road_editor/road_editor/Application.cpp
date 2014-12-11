#include "stdafx.h"
#include "Application.h"
#include "CommandConsoleSystem.h"
#include "NamedProperties.h"
#include "EventSystem.h"
#include "TextureManager.h"
#include "Vector4.h"
#include "Vector3.hpp"
#include "RoadSystem.h"
#include "Vehicle.h"
#include "FileManager.h"
#include "Road.h"
#include "LaneCluster.h"
#include "DrivingLane.h"
#include "LaneNode.h"
#include "Intersection.h"
#include "Camera.h"
#include "Viewport.h"
#include "IntersectionConnection.h"
#include "VehicleManager.h"
#include "KeyPadInputLogger.h"

namespace
{
	const int MAX_CAR_NUMBER = 24;

	//helper functions
	void ConsoleCommand_ClearConsole( const ConsoleCommandArgs& args )
	{
		//this function does nothing with the command arguments
		theConsole.clearScreen();
	}

	void ConsoleCommand_PrintList( const ConsoleCommandArgs& args )
	{
		std::for_each( args.m_argumentsVector.begin(), args.m_argumentsVector.end(), [&]( const std::string& line )
		{
			theConsole.printNewlineToConsole( line, toRGBA( 1.f, 1.f, 1.f ) );
		});
	}

	void ConsoleCommand_QuitApplication( const ConsoleCommandArgs& args )
	{
		//this function does nothing with the command line arguments
		PostQuitMessage( 0 );
	}

	void ConsoleCommand_PrintLine( const ConsoleCommandArgs& args )
	{
		theConsole.printNewlineToConsole( args.m_argumentsString, toRGBA( 1.f, 1.f, 1.f ) );
	}
	
	void registerConsoleCommand( const std::string& cmdName, ConsoleCommandFunctionPtr CommandFunction )
	{
		std::string cmdNameLower = cmdName;
		std::transform( cmdNameLower.begin(), cmdNameLower.end(), cmdNameLower.begin(), ::tolower );
		theConsole.registerConsoleCommand( cmdNameLower, CommandFunction );
	}

	void ExecuteConsoleCommand( const std::string& cmdName, const std::string& argumentsString )
	{
		theConsole.executeConsoleCommandString( cmdName + " " + argumentsString );
	}

	void printNewlineToConsole( const std::string& toPrint, unsigned int tint = 0 )
	{
		theConsole.printNewlineToConsole( toPrint, tint );
	}

	gh::KeyPadInputLogger* s_inputLogger = new gh::KeyPadInputLogger();

	bool g_renderIntersections = true;
	bool g_renderRoadNodeClusters = true;
	bool g_renderTempNodes = true;
}

namespace gh
{
	void RoadNode::Render(MatrixStack& matrixStack, const Vector3& nodeColor, float sizeMultiplier)
	{
		matrixStack.PushMatrix();
		matrixStack.Translate(m_location);

		glLoadMatrixf( matrixStack.GetTopMatrixFV() );

		Rgba myColor = Rgba::WHITE;
		myColor.m_R = nodeColor.x;
		myColor.m_G = nodeColor.y;
		myColor.m_B = nodeColor.z;

		AABB2 renderQuad(-1.f * sizeMultiplier, -1.f * sizeMultiplier, 1.f * sizeMultiplier, 1.f * sizeMultiplier);
		theRenderer.renderQuad(renderQuad, myColor.getVector4(), nullptr);
		matrixStack.PopMatrix();
	}

	void RoadNode::SetLocation(const Vector3& newLocation)
	{
		m_location = newLocation;
	}

	void RoadNode::AddNextNode(RoadNode* nodeToAdd)
	{
		//check that the node is not in our list
		for(int i = 0; i < m_nextNodes.size(); ++i)
		{
			if(m_nextNodes[i] == nodeToAdd)
			{
				return;
			}
		}

		m_nextNodes.push_back(nodeToAdd);
	}

	void RoadNode::AddPreviousNode(RoadNode* nodeToAdd)
	{
		//check that the node is not in our list
		for(int i = 0; i < m_previousNodes.size(); ++i)
		{
			if(m_previousNodes[i] == nodeToAdd)
			{
				return;
			}
		}

		m_previousNodes.push_back(nodeToAdd);
	}

	void RoadNode::RemoveNextNode(RoadNode* nodeToRemove)
	{
		int indexOfNode = -1;
		for(int index = 0; index < m_nextNodes.size(); ++index)
		{
			if(nodeToRemove == m_nextNodes[index])
			{
				indexOfNode = index;
				break;
			}
		}

		if(indexOfNode > -1)
		{
			m_nextNodes.erase(m_nextNodes.begin() + indexOfNode);
		}
	}

	Vector3 RoadNode::GetTangentOfNode()
	{
		Vector3 tangent;

		if( !m_nextNodes.empty()
			&& m_nextNodes[0] != nullptr 
			&& m_nextNodes[0]->m_isValid)
		{
			tangent = m_nextNodes[0]->m_location - m_location;
		}
		else if( !m_previousNodes.empty() 
					&& m_previousNodes[0] != nullptr
					&& m_previousNodes[0]->m_isValid)
		{
			tangent = m_location - m_previousNodes[0]->m_location;
		}

		return tangent;
	}

	//TODO:
	//Intersections should not have a null incoming, nor a null outgoing node
	void RoadNodeIntersection::Render( MatrixStack& matrixStack, const Vector3& nodeColor /*= Vector3(1.f, 1.f, 1.f)*/, float sizeMultiplier )
	{
		matrixStack.PushMatrix();
		matrixStack.Translate(m_location);

		glLoadMatrixf( matrixStack.GetTopMatrixFV() );

		Rgba myColor = Rgba::WHITE;
		myColor.m_R = 0.5f;
		myColor.m_G = 0.f;
		myColor.m_B = 0.5f;

		AABB2 test(-1, -1, 1, 1);
		theRenderer.renderQuad(test, myColor.getVector4(), nullptr);

		matrixStack.PopMatrix();

		glLoadMatrixf(matrixStack.GetTopMatrixFV());
		glBegin(GL_LINES);
		{
			glColor3f(0.f,0.5f,0.5f);

			Vector3 currentNodeLocation;
			for(int i = 0; i < m_previousNodes.size(); ++i)
			{
				if(m_previousNodes[i] != nullptr)
				{
					currentNodeLocation = m_previousNodes[i]->m_location;
					glVertex3f(currentNodeLocation.x, currentNodeLocation.y, currentNodeLocation.z);
					glVertex3f(m_location.x, m_location.y, m_location.z);
				}
			}

			glColor3f(0.5f, 0.5f, 0.f);
			for(int i = 0; i < m_nextNodes.size(); ++i)
			{
				if(m_nextNodes[i] != nullptr)
				{
					currentNodeLocation = m_nextNodes[i]->m_location;
					glVertex3f(m_location.x, m_location.y, m_location.z);
					glVertex3f(currentNodeLocation.x, currentNodeLocation.y, currentNodeLocation.z);
				}
			}
		}
		glEnd();
	}

	void RoadNodeIntersection::AddIncomingRoadNode(RoadNode* incomingRoadNode)
	{
		m_intersectionConnectionsMap[incomingRoadNode] = m_nextNodes;
		m_previousNodes.push_back(incomingRoadNode);
	}

	void RoadNodeIntersection::AddOutgoingRoadNode(RoadNode* outgoingRoadNode)
	{
		for(int i = 0; i < m_previousNodes.size(); ++i)
		{
			m_intersectionConnectionsMap[m_previousNodes[i]].push_back(outgoingRoadNode);
		}

		m_nextNodes.push_back(outgoingRoadNode);
	}

	bool RoadNodeIntersection::AllowsForOutgoingNodes()
	{
		//TODO:
		//Write implementation
		return true;
	}

	void RoadNodeIntersection::GetDirectionOfBranchingSegments(std::vector<Vector3>& out_directionSegments)
	{
		for(int i = 0; i < m_nextNodes.size(); ++i)
		{
			out_directionSegments.push_back( m_nextNodes[i]->m_location - m_location );
		}
	}

	Vector3 RoadNodeIntersection::GetDefaultDirectionOfIntersection()
	{
		//the first connecting road is the one that specifies the default firection of the intersection
		Vector3 defaultDirectionOfIntersection = m_location - m_previousNodes[0]->m_location;
		return defaultDirectionOfIntersection;
	}

	void RoadNodeCluster::AddNode(RoadNode* nodeToAdd)
	{
		m_roadNodes.push_back(nodeToAdd);
	}

	void RoadNodeCluster::Render(MatrixStack& matrixStack, const Vector3& nodeColor, bool renderDirection)
	{
		matrixStack.PushMatrix();

		//render the nodes
		for(int i = 0; i < m_roadNodes.size(); ++i)
		{
			m_roadNodes[i]->Render(matrixStack, nodeColor);
		}

		std::vector<NodeInformation> arrowheads;
		NodeInformation currentArrowHeadInformation;
		int nodesBetweenArrowHeads = 5;
		int nodeCount = 0;

		//render the connecting lines
		glLoadMatrixf(matrixStack.GetTopMatrixFV());
		glDisable(GL_TEXTURE_2D);
		glUseProgram(0);

		glBegin(GL_LINES);
		{
			glColor3f(1.f,0.f,0.f);

			Vector3 currentNodeLocation;
			Vector3 nextNodeLocation;
			RoadNode* currentNode = nullptr;
			RoadNode* nextNodePtr = nullptr;
			for(int i = 0; i < m_roadNodes.size(); ++i)
			{
				currentNode = m_roadNodes[i];
				currentNodeLocation = currentNode->m_location;

				for(int nextNodeIndex = 0; nextNodeIndex < currentNode->m_nextNodes.size(); ++nextNodeIndex)
				{
					nextNodePtr = currentNode->m_nextNodes[nextNodeIndex];

					if(nextNodePtr)
					{
						nextNodeLocation = nextNodePtr->m_location;
						glVertex3f(currentNodeLocation.x, currentNodeLocation.y, currentNodeLocation.z);
						glVertex3f(nextNodeLocation.x, nextNodeLocation.y, nextNodeLocation.z);
					}
				}
				
				++nodeCount;
				if(nodeCount >= nodesBetweenArrowHeads && renderDirection)
				{
					currentArrowHeadInformation.location = currentNodeLocation;
					currentArrowHeadInformation.direction = nextNodeLocation - currentNodeLocation;
					currentArrowHeadInformation.direction.normalize();
					arrowheads.push_back(currentArrowHeadInformation);
					nodeCount = 0;
				}
			}
		}
		glEnd();


		if(arrowheads.size() > 0)
		{
			glBegin(GL_TRIANGLES);
			{
				glColor3f(0.f,1.f,0.f);

				Vector3 point1;
				Vector3 point2;
				Vector3 point3;
				Vector3 rotatedCWDirection;
				Matrix4X4 rotationMatrix90CW = Matrix4X4::RotateZDegreesMatrix(-90.f);
				for(int i = 0; i < arrowheads.size(); ++i)
				{
					currentArrowHeadInformation = arrowheads[i];
					point1 = currentArrowHeadInformation.location;
					point2 = currentArrowHeadInformation.location - (currentArrowHeadInformation.direction * 10.f);
					point3 = currentArrowHeadInformation.location - (currentArrowHeadInformation.direction * 10.f);

					rotatedCWDirection = rotationMatrix90CW.TransformDirection(currentArrowHeadInformation.direction);
					point2 -= (rotatedCWDirection * 10.f * 0.5f);
					point3 += (rotatedCWDirection * 10.f * 0.5f);

					glVertex3f(point1.x, point1.y, point1.z);
					glVertex3f(point2.x, point2.y, point2.z);
					glVertex3f(point3.x, point3.y, point3.z);
				}
			}
			glEnd();
		}

		//reset the color back
		glColor3f( 1.f, 1.f, 1.f );

		matrixStack.PopMatrix();
	}

	//TODO
	//Look for the cases in which this can cause an error because the index of road node is == 0
	Vector3 RoadNodeCluster::GetTangentOfNodeAtSpecifiedIndex( int indexOfRoadNode )
	{
		Vector3 tangent;
		if(indexOfRoadNode < m_roadNodes.size() - 1)
		{
			tangent = m_roadNodes[indexOfRoadNode + 1]->m_location - m_roadNodes[indexOfRoadNode]->m_location;
		}
		else
		{
			tangent = m_roadNodes[indexOfRoadNode]->m_location - m_roadNodes[indexOfRoadNode - 1]->m_location;
		}

		return tangent;
	}

	void RoadNodeCluster::InitiateNodeConnections()
	{
		RoadNode* prevNode = nullptr;

		for(int nodeIndex = 0; nodeIndex < m_roadNodes.size(); ++nodeIndex)
		{
			if(prevNode)
			{
				prevNode->AddNextNode(m_roadNodes[nodeIndex]);
			}

			m_roadNodes[nodeIndex]->AddPreviousNode(prevNode);

			prevNode = m_roadNodes[nodeIndex];
		}
	}


	Application::Application( HWND handleWnd, HDC handleDC )
		:	m_hWnd( handleWnd )
		,	m_hDC( handleDC )
		,	m_openCommandConsole( false )
		,	m_displayOrigin( false )
		,	m_matrixStack( MatrixStack() )
		,	m_windowSize( Vector2() )
		,	m_mouseScreenCoordinates( Vector3() )
		,	m_mouseNearFrustumCoordinates( Vector3() )
		,	m_mouseFarFrustumCoordinates( Vector3() )
		,	m_splineMode(false)
		,	m_backgroundColor( Rgba( 0.0f, 0.0f, 0.0f ) )
		,	m_roadSystem( nullptr )
		,	m_camera( nullptr )
		,	m_viewport( nullptr )
		,	m_vehicleManager( nullptr )
		,	m_updateInput( false )
		,	m_stop( false )
		,	m_nearZ(0.1f)
		,	m_farZ(10000.f)
		,	m_fovy(45.f)
		,	m_aspectRatio(0.f)
		,	m_indexOfLastPermanentNode(0)
		,	m_indexOfLastTempNode(-1)
		,	m_lengthOfFragment(10.f)
		,	m_lengthOfFragmentSquared(m_lengthOfFragment * m_lengthOfFragment)
		,	m_currentRoadNodeCluster(nullptr)
		,	m_maxYawRotationDegreesForRoadSegments(8.f)
		,	m_maxTurnAngleDotProductForRoadSegments(0.f)
		,	m_HUDFontHeight(20)
		,	m_HUDLineBreakHeight(5)
		,	m_showSecondCurveSystem(false)
		,	m_showDirectionOnPlacedRoads(false)
		,	m_showDirectionOnTempNodes(false)
		,	m_roadNodeInRange(nullptr)
		,	m_intersectionNodeInRange(nullptr)
		,	m_roadNodeClusterInRange(nullptr)
		,	m_intersectionNodeIndex(-1)
		,	m_currentRoadIsNew(false)
		,	m_forkIntersectionNode(nullptr)
		,	m_roadNodeClusterToSpawnFork(nullptr)
		,	m_roadNodeIntersectionToForkFrom(nullptr)
		,	m_indexOfForkNode(-1)
	{
		//initialize glew
		glewInit();

		//Set up file manager to read necessary data files
		FileManager::getSingleton().setDiskDataLocation( "..\\..\\..\\TempEngine\\Data" );
		FileManager::getSingleton().setArchiveLocation( "..\\..\\..\\TempEngine\\Data", "password" );
		FileManager::getSingleton().setLoadingMode( PREFER_DATA );

		//calculate the window size and store it
		storeWindowSize( m_hWnd );

		//creating the Texture manager, shader loader and initializing the icoSpehere
		TextureManager::create();

		glEnable( GL_CULL_FACE );
		glFrontFace( GL_CCW );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable( GL_BLEND );

		//load font
		std::vector< std::string > glyphSheets;
		glyphSheets.push_back("Fonts\\MainFont_EN_00.png" );
		theRenderer.loadFont( "Fonts\\MainFont_EN.FontDef.xml", "test", glyphSheets );
		
		//tell the console what font to use
		theConsole.init( "   >", "test", 24.f );

		//registering the commands to the console
		registerConsoleCommand( "PRINTLINE", &( ConsoleCommand_PrintLine ) );
		registerConsoleCommand( "CLEAR", &( ConsoleCommand_ClearConsole ) );
		registerConsoleCommand( "PRINTLIST", &( ConsoleCommand_PrintList ) );
		registerConsoleCommand( "QUIT", &( ConsoleCommand_QuitApplication ) );

		//initiate driving system and vehicle manager
		//initiateDrivingSystem();

		m_camera = new Camera( false );
		m_camera->setPosition( Vector3( 0.f, 0.f, 100.f ) );

		m_updateInput = true;

		m_rotationMatrix90DegreesCW = Matrix4X4::RotateZDegreesMatrix(-90.f);
		PrecalculateRoadVariables();
	}

	void Application::PrecalculateRoadVariables()
	{
		//road system variables
		Vector3 unitVector(1.f, 0.f, 0.f);
		Vector3 rotatedVector = unitVector;
		rotatedVector.setPitchAndYawDegrees(m_maxYawRotationDegreesForRoadSegments, 0.f);

		m_maxTurnAngleDotProductForRoadSegments = rotatedVector.DotProduct(unitVector);
		m_roadMaxCWRotationTransformationMatrix = Matrix4X4::RotateZDegreesMatrix(-m_maxYawRotationDegreesForRoadSegments);
		m_roadMaxCCWRotationTransformationMatrix = Matrix4X4::RotateZDegreesMatrix(m_maxYawRotationDegreesForRoadSegments);

		//estimate the diameter for a full turn
		Vector3 origin;
		Vector3 finalLocation;
		Vector3 prevLocation;
		Vector3 direction(1.f,0.f,0.f);
		Vector3 desiredDirection(-1.f,0.f,0.f);
		float currentDotProduct = 0.f;
		float prevDotProduct = -1.f;
		bool continueLooping = true;

		while(continueLooping)
		{
			currentDotProduct = direction.DotProduct(desiredDirection);
			if(currentDotProduct < prevDotProduct)
			{
				continueLooping = false;
				continue;
			}
			else
			{
				prevDotProduct = currentDotProduct;
				direction = m_roadMaxCWRotationTransformationMatrix.TransformDirection(direction);
				prevLocation = finalLocation;
				finalLocation += (direction * m_lengthOfFragment);
			}
		}

		m_radiusOfRoadCircle = prevLocation.calculateRadialDistance() * .5f;
	}

	void Application::onResize( int width, int height )
	{
		//keep track of the new window size
		m_windowSize.x = (float)width;
		m_windowSize.y = (float)height;
		theRenderer.updateWindowSize(m_windowSize);
		theConsole.recalculateText();

		glViewport( 0, 0, width, height );
		if(m_viewport == nullptr)
		{
			m_viewport = new Viewport(0, 0, width, height);
		}
		else
		{
			m_viewport->SetViewport(0, 0, width, height);
		}

		m_aspectRatio = width / ((float)height);

		glMatrixMode( GL_MODELVIEW );
		m_matrixStack.ClearStack();
		m_matrixStack.LoadIdentity();
		m_matrixStack.PushPerspective( m_fovy, m_aspectRatio, m_nearZ, m_farZ );
	}

	//TODO:
	//REPLACE THIS WITH SOME SORT OF FILE READING AND PARSING
	//THIS IS AN AWFUL WAY TO SET UP THE ROAD SYSTEM
	void Application::initiateRoadSystem()
	{
		m_roadSystem = new RoadSystem();

		Road* road1 = new Road();
		
		LaneCluster* cluster1_1 = new LaneCluster();
		DrivingLane* lane1_1_1 = new DrivingLane( "lane1_1_1" );

		LaneCluster* cluster1_2 = new LaneCluster();
		DrivingLane* lane1_2_1 = new DrivingLane( "lane1_2_1" );

		
		Road* road2 = new Road();
		
		LaneCluster* cluster2_1 = new LaneCluster();
		DrivingLane* lane2_1_1 = new DrivingLane( "lane2_1_1" );

		LaneCluster* cluster2_2 = new LaneCluster();
		DrivingLane* lane2_2_1 = new DrivingLane( "lane2_2_1" );


		Road* road3 = new Road();

		LaneCluster* cluster3_1 = new LaneCluster();
		DrivingLane* lane3_1_1 = new DrivingLane( "lane3_1_1" );

		LaneCluster* cluster3_2 = new LaneCluster();
		DrivingLane* lane3_2_1 = new DrivingLane( "lane3_2_1" );


		Road* road4 = new Road();

		LaneCluster* cluster4_1 = new LaneCluster();
		DrivingLane* lane4_1_1 = new DrivingLane( "lane4_1_1" );

		LaneCluster* cluster4_2 = new LaneCluster();
		DrivingLane* lane4_2_1 = new DrivingLane( "lane4_2_1" );


		Road* road5 = new Road();

		LaneCluster* cluster5_1 = new LaneCluster();
		DrivingLane* lane5_1_1 = new DrivingLane( "lane5_1_1" );

		LaneCluster* cluster5_2 = new LaneCluster();
		DrivingLane* lane5_2_1 = new DrivingLane( "lane5_2_1" );


		Road* road6 = new Road();

		LaneCluster* cluster6_1 = new LaneCluster();
		DrivingLane* lane6_1_1 = new DrivingLane( "lane6_1_1" );

		LaneCluster* cluster6_2 = new LaneCluster();
		DrivingLane* lane6_2_1 = new DrivingLane( "lane6_2_1" );


		Road* road7 = new Road();

		LaneCluster* cluster7_1 = new LaneCluster();
		DrivingLane* lane7_1_1 = new DrivingLane( "lane7_1_1" );

		LaneCluster* cluster7_2 = new LaneCluster();
		DrivingLane* lane7_2_1 = new DrivingLane( "lane7_2_1" );


		Road* road8 = new Road();

		LaneCluster* cluster8_1 = new LaneCluster();
		DrivingLane* lane8_1_1 = new DrivingLane( "lane8_1_1" );

		LaneCluster* cluster8_2 = new LaneCluster();
		DrivingLane* lane8_2_1 = new DrivingLane( "lane8_2_1" );



		Road* road9 = new Road();

		LaneCluster* cluster9_1 = new LaneCluster();
		DrivingLane* lane9_1_1 = new DrivingLane( "lane9_1_1" );

		LaneCluster* cluster9_2 = new LaneCluster();
		DrivingLane* lane9_2_1 = new DrivingLane( "lane9_2_1" );



		Road* road10 = new Road();

		LaneCluster* cluster10_1 = new LaneCluster();
		DrivingLane* lane10_1_1 = new DrivingLane( "lane10_1_1" );

		LaneCluster* cluster10_2 = new LaneCluster();
		DrivingLane* lane10_2_1 = new DrivingLane( "lane10_2_1" );



		Road* road11 = new Road();

		LaneCluster* cluster11_1 = new LaneCluster();
		DrivingLane* lane11_1_1 = new DrivingLane( "lane11_1_1" );

		LaneCluster* cluster11_2 = new LaneCluster();
		DrivingLane* lane11_2_1 = new DrivingLane( "lane11_2_1" );






		LaneNode* myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 0.f, 0.f, 0.f);
		lane1_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 50.f, 0.f, 0.f );
		lane1_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 100.f, 0.f, 0.f );
		lane1_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 150.f, 0.f, 0.f );
		lane1_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 200.f, 0.f, 0.f );
		lane1_1_1->pushLaneNode(myLaneNode);

		cluster1_1->pushDrivingLane( lane1_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 200.f, 50.f, 0.f);
		lane1_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 150.f, 50.f, 0.f );
		lane1_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 100.f, 50.f, 0.f );
		lane1_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 50.f, 50.f, 0.f );
		lane1_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 0.f, 50.f, 0.f );
		lane1_2_1->pushLaneNode(myLaneNode);

		cluster1_2->pushDrivingLane( lane1_2_1 );

		road1->pushLaneCluster( cluster1_1 );
		road1->pushLaneCluster( cluster1_2 );

		m_roadSystem->addRoad(road1);




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 350.f, 0.f);
		lane2_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 300.f, 0.f );
		lane2_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 250.f, 0.f );
		lane2_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 200.f, 0.f );
		lane2_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 150.f, 0.f );
		lane2_1_1->pushLaneNode(myLaneNode);

		cluster2_1->pushDrivingLane( lane2_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 150.f, 0.f);
		lane2_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 200.f, 0.f );
		lane2_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 250.f, 0.f );
		lane2_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 300.f, 0.f );
		lane2_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 350.f, 0.f );
		lane2_2_1->pushLaneNode(myLaneNode);

		cluster2_2->pushDrivingLane( lane2_2_1 );

		road2->pushLaneCluster( cluster2_1 );
		road2->pushLaneCluster( cluster2_2 );

		m_roadSystem->addRoad( road2 );




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 450.f, 0.f, 0.f);
		lane3_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 500.f, 0.f, 0.f );
		lane3_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 550.f, 0.f, 0.f );
		lane3_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 600.f, 0.f, 0.f );
		lane3_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 650.f, 0.f, 0.f );
		lane3_1_1->pushLaneNode(myLaneNode);

		cluster3_1->pushDrivingLane( lane3_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 650.f, 50.f, 0.f);
		lane3_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 600.f, 50.f, 0.f );
		lane3_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 550.f, 50.f, 0.f );
		lane3_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 500.f, 50.f, 0.f );
		lane3_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 450.f, 50.f, 0.f );
		lane3_2_1->pushLaneNode(myLaneNode);

		cluster3_2->pushDrivingLane( lane3_2_1 );

		road3->pushLaneCluster( cluster3_1 );
		road3->pushLaneCluster( cluster3_2 );

		m_roadSystem->addRoad(road3);




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, -100.f, 0.f);
		lane4_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, -150.f, 0.f );
		lane4_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, -200.f, 0.f );
		lane4_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, -250.f, 0.f );
		lane4_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, -300.f, 0.f );
		lane4_1_1->pushLaneNode(myLaneNode);

		cluster4_1->pushDrivingLane( lane4_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, -300.f, 0.f);
		lane4_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, -250.f, 0.f );
		lane4_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, -200.f, 0.f );
		lane4_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, -150.f, 0.f );
		lane4_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, -100.f, 0.f );
		lane4_2_1->pushLaneNode(myLaneNode);

		cluster4_2->pushDrivingLane( lane4_2_1 );

		road4->pushLaneCluster( cluster4_1 );
		road4->pushLaneCluster( cluster4_2 );

		m_roadSystem->addRoad( road4 );




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 350.f, 0.f);
		lane5_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 300.f, 0.f );
		lane5_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 250.f, 0.f );
		lane5_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 200.f, 0.f );
		lane5_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 150.f, 0.f );
		lane5_1_1->pushLaneNode(myLaneNode);

		cluster5_1->pushDrivingLane( lane5_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 150.f, 0.f);
		lane5_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 200.f, 0.f );
		lane5_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 250.f, 0.f );
		lane5_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 300.f, 0.f );
		lane5_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 350.f, 0.f );
		lane5_2_1->pushLaneNode(myLaneNode);

		cluster5_2->pushDrivingLane( lane5_2_1 );

		road5->pushLaneCluster( cluster5_1 );
		road5->pushLaneCluster( cluster5_2 );

		m_roadSystem->addRoad( road5 );




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 150.f, 0.f);
		lane6_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 200.f, 0.f );
		lane6_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 250.f, 0.f );
		lane6_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 300.f, 0.f );
		lane6_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 350.f, 0.f );
		lane6_1_1->pushLaneNode(myLaneNode);

		cluster6_1->pushDrivingLane( lane6_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 350.f, 0.f);
		lane6_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 300.f, 0.f );
		lane6_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 250.f, 0.f );
		lane6_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 200.f, 0.f );
		lane6_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 150.f, 0.f );
		lane6_2_1->pushLaneNode(myLaneNode);

		cluster6_2->pushDrivingLane( lane6_2_1 );

		road6->pushLaneCluster( cluster6_1 );
		road6->pushLaneCluster( cluster6_2 );

		m_roadSystem->addRoad( road6 );




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 650.f, 500.f, 0.f);
		lane7_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 600.f, 500.f, 0.f );
		lane7_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 550.f, 500.f, 0.f );
		lane7_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 500.f, 500.f, 0.f );
		lane7_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 450.f, 500.f, 0.f );
		lane7_1_1->pushLaneNode(myLaneNode);

		cluster7_1->pushDrivingLane( lane7_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 450.f, 450.f, 0.f);
		lane7_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 500.f, 450.f, 0.f );
		lane7_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 550.f, 450.f, 0.f );
		lane7_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 600.f, 450.f, 0.f );
		lane7_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 650.f, 450.f, 0.f );
		lane7_2_1->pushLaneNode(myLaneNode);

		cluster7_2->pushDrivingLane( lane7_2_1 );

		road7->pushLaneCluster( cluster7_1 );
		road7->pushLaneCluster( cluster7_2 );

		m_roadSystem->addRoad( road7 );




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 200.f, 500.f, 0.f);
		lane8_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 150.f, 500.f, 0.f );
		lane8_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 100.f, 500.f, 0.f );
		lane8_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 50.f, 500.f, 0.f );
		lane8_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 0.f, 500.f, 0.f );
		lane8_1_1->pushLaneNode(myLaneNode);

		cluster8_1->pushDrivingLane( lane8_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 0.f, 450.f, 0.f);
		lane8_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 50.f, 450.f, 0.f );
		lane8_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 100.f, 450.f, 0.f );
		lane8_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 150.f, 450.f, 0.f );
		lane8_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 200.f, 450.f, 0.f );
		lane8_2_1->pushLaneNode(myLaneNode);

		cluster8_2->pushDrivingLane( lane8_2_1 );

		road8->pushLaneCluster( cluster8_1 );
		road8->pushLaneCluster( cluster8_2 );

		m_roadSystem->addRoad( road8 );




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 600.f, 0.f);
		lane9_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 650.f, 0.f );
		lane9_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 700.f, 0.f );
		lane9_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 750.f, 0.f );
		lane9_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -100.f, 800.f, 0.f );
		lane9_1_1->pushLaneNode(myLaneNode);

		cluster9_1->pushDrivingLane( lane9_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 800.f, 0.f);
		lane9_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 750.f, 0.f );
		lane9_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 700.f, 0.f );
		lane9_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 650.f, 0.f );
		lane9_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( -150.f, 600.f, 0.f );
		lane9_2_1->pushLaneNode(myLaneNode);

		cluster9_2->pushDrivingLane( lane9_2_1 );

		road9->pushLaneCluster( cluster9_1 );
		road9->pushLaneCluster( cluster9_2 );

		m_roadSystem->addRoad( road9 );




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 600.f, 0.f);
		lane10_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 650.f, 0.f );
		lane10_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 700.f, 0.f );
		lane10_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 750.f, 0.f );
		lane10_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 350.f, 800.f, 0.f );
		lane10_1_1->pushLaneNode(myLaneNode);

		cluster10_1->pushDrivingLane( lane10_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 800.f, 0.f);
		lane10_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 750.f, 0.f );
		lane10_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 700.f, 0.f );
		lane10_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 650.f, 0.f );
		lane10_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 300.f, 600.f, 0.f );
		lane10_2_1->pushLaneNode(myLaneNode);

		cluster10_2->pushDrivingLane( lane10_2_1 );

		road10->pushLaneCluster( cluster10_1 );
		road10->pushLaneCluster( cluster10_2 );

		m_roadSystem->addRoad( road10 );




		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 600.f, 0.f);
		lane11_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 650.f, 0.f );
		lane11_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 700.f, 0.f );
		lane11_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 750.f, 0.f );
		lane11_1_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 800.f, 800.f, 0.f );
		lane11_1_1->pushLaneNode(myLaneNode);

		cluster11_1->pushDrivingLane( lane11_1_1 );


		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 800.f, 0.f);
		lane11_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 750.f, 0.f );
		lane11_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 700.f, 0.f );
		lane11_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 650.f, 0.f );
		lane11_2_1->pushLaneNode(myLaneNode);

		myLaneNode = new LaneNode();
		myLaneNode->m_position = Vector3( 750.f, 600.f, 0.f );
		lane11_2_1->pushLaneNode(myLaneNode);

		cluster11_2->pushDrivingLane( lane11_2_1 );

		road11->pushLaneCluster( cluster11_1 );
		road11->pushLaneCluster( cluster11_2 );

		m_roadSystem->addRoad( road11 );







		std::vector< IntersectionConnection* > synchronizedLanes;

		//add the intersection between the roads
		IntersectionConnection* testIntersection = new IntersectionConnection( lane6_2_1, lane1_1_1 );
		synchronizedLanes.push_back( testIntersection );
		Intersection* intersectionCollection = new Intersection();
		intersectionCollection->addIntersectionConnections( synchronizedLanes, ALWAYS_STOP );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane1_2_1, lane6_1_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections( synchronizedLanes, ALWAYS_STOP );
		synchronizedLanes.clear();

		//add the intersection collection to the roadSystem
		m_roadSystem->addIntersection( intersectionCollection );
		intersectionCollection = new Intersection();



		testIntersection = new IntersectionConnection( lane1_1_1, lane2_2_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane3_2_1, lane4_1_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane1_1_1, lane3_1_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane3_2_1, lane1_2_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, SIGNAL_LIGHT, 14.f, 5.f );
		synchronizedLanes.clear();



		testIntersection = new IntersectionConnection( lane2_1_1, lane3_1_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane4_2_1, lane1_2_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane4_2_1, lane2_2_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane2_1_1, lane4_1_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, SIGNAL_LIGHT, 14.f, 5.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane4_2_1, lane3_1_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane3_2_1, lane2_2_1 );
		synchronizedLanes.push_back(testIntersection);
		testIntersection = new IntersectionConnection( lane2_1_1, lane1_2_1 );
		synchronizedLanes.push_back(testIntersection);
		testIntersection = new IntersectionConnection( lane1_1_1, lane4_1_1);
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, ALWAYS_STOP );
		synchronizedLanes.clear();

		m_roadSystem->addIntersection( intersectionCollection );
		intersectionCollection = new Intersection();



		//add the intersection between the roads
		testIntersection = new IntersectionConnection( lane5_1_1, lane3_2_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections( synchronizedLanes, ALWAYS_STOP );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane3_1_1, lane5_2_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections( synchronizedLanes, ALWAYS_STOP );
		synchronizedLanes.clear();

		m_roadSystem->addIntersection( intersectionCollection );
		intersectionCollection = new Intersection();



		testIntersection = new IntersectionConnection( lane7_2_1, lane11_1_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();


		testIntersection = new IntersectionConnection( lane5_2_1, lane7_1_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane5_2_1, lane11_1_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane11_2_1, lane5_1_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, SIGNAL_LIGHT, 14.f, 5.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane11_2_1, lane7_1_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane7_2_1, lane5_1_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, ALWAYS_STOP );
		synchronizedLanes.clear();

		m_roadSystem->addIntersection( intersectionCollection );
		intersectionCollection = new Intersection();



		testIntersection = new IntersectionConnection( lane8_2_1, lane10_1_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane7_1_1, lane2_1_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane8_2_1, lane7_2_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane7_1_1, lane8_1_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, SIGNAL_LIGHT, 14.f, 5.f );
		synchronizedLanes.clear();



		testIntersection = new IntersectionConnection( lane10_2_1, lane7_2_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane2_2_1, lane8_1_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane2_2_1, lane10_1_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane10_2_1, lane2_1_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, SIGNAL_LIGHT, 14.f, 5.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane2_2_1, lane7_2_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane7_1_1, lane10_1_1 );
		synchronizedLanes.push_back(testIntersection);
		testIntersection = new IntersectionConnection( lane10_2_1, lane8_1_1 );
		synchronizedLanes.push_back(testIntersection);
		testIntersection = new IntersectionConnection( lane8_2_1, lane2_1_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, ALWAYS_STOP );
		synchronizedLanes.clear();

		m_roadSystem->addIntersection( intersectionCollection );
		intersectionCollection = new Intersection();



		testIntersection = new IntersectionConnection( lane8_1_1, lane6_2_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();


		testIntersection = new IntersectionConnection( lane9_2_1, lane8_2_1 );
		synchronizedLanes.push_back( testIntersection );
		intersectionCollection->addIntersectionConnections(synchronizedLanes, SIGNAL_LIGHT, 5.f, 2.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane9_2_1, lane6_2_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane6_1_1, lane9_1_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, SIGNAL_LIGHT, 14.f, 5.f );
		synchronizedLanes.clear();

		testIntersection = new IntersectionConnection( lane8_1_1, lane9_1_1 );
		synchronizedLanes.push_back( testIntersection );
		testIntersection = new IntersectionConnection( lane6_1_1, lane8_2_1 );
		synchronizedLanes.push_back(testIntersection);
		intersectionCollection->addIntersectionConnections( synchronizedLanes, ALWAYS_STOP );
		synchronizedLanes.clear();

		m_roadSystem->addIntersection( intersectionCollection );
	}

	void Application::drawOrigin( float lineLength )
	{
		glDisable( GL_TEXTURE_2D );
		glUseProgram( 0 );

		glBegin( GL_LINES );
		{
			glColor3f(   1,  0,  0 );
			glVertex3f(	 0, 0,	0);

			glColor3f(   1,  0,  0 );
			glVertex3f(	lineLength,	0,	0 );

			glColor3f(   0,  1,  0 );
			glVertex3f(  0,  0,  0 );

			glColor3f(   0,  1,  0 );
			glVertex3f(  0,  lineLength,  0 );

			glColor3f(   0,  0,  1 );
			glVertex3f(  0,  0,  0 );

			glColor3f(   0,  0,  1 );
			glVertex3f(  0,  0,   lineLength );
		}
		glEnd();

		//reset the color back
		glColor3f( 1.f, 1.f, 1.f );
	}

	void Application::UpdateCursorPosition()
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			if (ScreenToClient(m_hWnd, &p))
			{
				m_mouseScreenCoordinates.x = p.x;
				m_mouseScreenCoordinates.y = theRenderer.getWindowSize().y - p.y;

				//get the points that define the ray the mouse click defines in the frustum
				CalculateFrustumPoints(m_mouseScreenCoordinates, m_mouseNearFrustumCoordinates,
					m_mouseFarFrustumCoordinates);
			}
		}
	}

	//function calculates the points the screen coordinates defines in the frustrum
	void Application::CalculateFrustumPoints(const Vector3& screenCoordinates, Vector3& nearFrustumWorldCoordinates,
		Vector3& farFrustumWorldCoordinates)
	{
		//need to unproject the coordinates into world
		const Vector3& camPosition = m_camera->position();
		Vector3 localScreenCoordinates = screenCoordinates;

		m_matrixStack.PushMatrix();
		m_matrixStack.LoadIdentity();
		m_matrixStack.PushPerspective(m_fovy, m_aspectRatio, m_nearZ, m_farZ);

		//load the view matrix
		m_matrixStack.Translate(-camPosition);

		//get normalized device coordinates
		Vector4 nearNormalizedDeviceCoordinates;
		Vector4 farNormalizedDeviceCoordinates;
		
		localScreenCoordinates.z = 0.0f;
		m_viewport->GetNormalizedDeviceCoordinates(nearNormalizedDeviceCoordinates, localScreenCoordinates);

		localScreenCoordinates.z = 1.0f;
		m_viewport->GetNormalizedDeviceCoordinates(farNormalizedDeviceCoordinates, localScreenCoordinates);

		//calculate the unprojection matrix
		Matrix4X4 unprojectionMatrix = m_matrixStack.back();
		if(unprojectionMatrix.InvertMatrix())
		{
			Vector4 nearWorldPosition;
			Vector4 farWorldPosition;
			
			nearWorldPosition = unprojectionMatrix.TransformPoint(nearNormalizedDeviceCoordinates);
			nearWorldPosition.w = 1.f / nearWorldPosition.w;
			nearWorldPosition.x *= nearWorldPosition.w;
			nearWorldPosition.y *= nearWorldPosition.w;
			nearWorldPosition.z *= nearWorldPosition.w;
			nearFrustumWorldCoordinates.setXYZ(nearWorldPosition.x, nearWorldPosition.y, nearWorldPosition.z);

			farWorldPosition = unprojectionMatrix.TransformPoint(farNormalizedDeviceCoordinates);
			farWorldPosition.w = 1.f / farWorldPosition.w;
			farWorldPosition.x *= farWorldPosition.w;
			farWorldPosition.y *= farWorldPosition.w;
			farWorldPosition.z *= farWorldPosition.w;
			farFrustumWorldCoordinates.setXYZ(farWorldPosition.x, farWorldPosition.y, farWorldPosition.z);
		}

		m_matrixStack.PopMatrix();
	}

	void Application::updateFrame( double deltaTime )
	{	
		//reset input
		UpdateKeyInput();
		s_inputLogger->UpdateKeyStates();

		if( !m_openCommandConsole && m_updateInput )
		{
			m_camera->updateInput();
			m_camera->updateDynamics( deltaTime );

			//update the roads and vehicles
			if(m_roadSystem)
			m_roadSystem->update( deltaTime );

			if(m_vehicleManager)
			m_vehicleManager->updateVehicles( deltaTime );
		}

		//update the cursor position
		UpdateCursorPosition();

		//update the spline
		CalculateSplineToMousePos();

		m_matrixStack.PushMatrix();
		theRenderer.useRegularFBO( m_backgroundColor.getVector4(), CLEAR_COLOR | CLEAR_DEPTH );

		render2DScene();
		render3DScene();

		m_matrixStack.PopMatrix();
		SwapBuffers(m_hDC);
	}

	void Application::toggleOrigin()
	{
		m_displayOrigin = !m_displayOrigin;
	}

	void Application::toggleCommandConsole()
	{
		m_openCommandConsole = !m_openCommandConsole;
	}

	void Application::render2DScene()
	{
		m_matrixStack.PushMatrix();

		m_matrixStack.PushOrtho(0.f, m_windowSize.x, 0.f, m_windowSize.y, -1.f, 1.f );
		glLoadMatrixf( m_matrixStack.GetTopMatrixFV() );

		theRenderer.disableAllTextures();

		//rendering mouse
		if( m_openCommandConsole )
		{
			theConsole.render( AABB2( Vector2( 0.f, 0.f ), m_windowSize ) );
		}
		else
		{
			DrawHUD();
		}

		glEnable( GL_DEPTH_TEST );
		m_matrixStack.PopMatrix();
	}

	void Application::render3DScene()
	{
		m_matrixStack.PushMatrix();

		//rotate the camera
		const Vector3& camPosition = m_camera->position();
		const Vector3& camRotation = m_camera->rotation();
		m_matrixStack.RotateX( -camRotation.x );
		m_matrixStack.RotateY( -camRotation.y );

		//translate the camera
		m_matrixStack.Translate( -camPosition );

		//load the current matrix
		glLoadMatrixf( m_matrixStack.GetTopMatrixFV() );

		glUseProgram(0);
		glDisable( GL_DEPTH_TEST );
		glDisable( GL_TEXTURE_2D );

		if(m_roadSystem)
		m_roadSystem->render( m_matrixStack );
		
		if(m_vehicleManager)
		m_vehicleManager->renderVehicles( m_matrixStack );

		RenderSplines();
		RenderDebugNodes();

		glEnable( GL_TEXTURE_2D );
		m_matrixStack.PopMatrix();
	}

	void Application::RenderDebugNodes()
	{
		for(int i = 0; i < m_debugNodesToRender.size(); ++i)
		{
			m_debugNodesToRender[i]->Render(m_matrixStack, Vector3(1.f, 1.f, 1.f), 0.25f);
			delete m_debugNodesToRender[i];
		}

		if(m_roadNodeInRange)
		{
			m_roadNodeInRange->Render(m_matrixStack, Vector3(0.94f, 1.f, 0.35));
		}

		m_debugNodesToRender.clear();
	}

	bool Application::GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(const Vector3& desiredDirection,
		const Matrix4X4& maxRotationMatrix, float maxTurnAngleDotProduct, int indexOfLastValidNode, NodeInformation& out_info)
	{
		//Get the needed directions normalized
		Vector3 currentNormalizedDirection = m_tempNodes[indexOfLastValidNode]->GetTangentOfNode();
		currentNormalizedDirection.normalize();
		if(currentNormalizedDirection == Vector3())
		{
			return false;
		}
		Vector3 desiredNormalizedDirection = desiredDirection;
		desiredNormalizedDirection.normalize();

		float dotProductDesiredDirectionComparedToCurrentDirection = desiredNormalizedDirection.DotProduct(currentNormalizedDirection);

		out_info.location = m_tempNodes[indexOfLastValidNode]->m_location;

		//rotate the next fragment to be placed
		//check if we can turn the road segment to face the cursor directly
		if(maxTurnAngleDotProduct < dotProductDesiredDirectionComparedToCurrentDirection)
		{
			out_info.location += desiredNormalizedDirection * m_lengthOfFragment;
			out_info.direction = desiredNormalizedDirection;
		}
		else	//we should rotate the road segment to its max angle towards the desired direction
		{
			currentNormalizedDirection = maxRotationMatrix.TransformDirection(currentNormalizedDirection);
			out_info.location += currentNormalizedDirection * m_lengthOfFragment;
			out_info.direction = currentNormalizedDirection;
		}

		return true;
	}

	int Application::AddQuarterCircle(int indexOfQuarterCircleStart, const Vector3& directionOfStartNode, const Vector3& goalLocation)
	{
		Vector3 tangent = directionOfStartNode;
		//we need at least two nodes in order to figure out our current direction
		if( tangent == Vector3(0.f, 0.f, 0.f) )
		{
			return 0;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////
		//The following calculates the direction we should rotate the fragments when building the semicircle
		Matrix4X4 rotationMatrix;
		RotationDirection directionToRotate = GetBestSuitedRotationMatrix(m_tempNodes[indexOfQuarterCircleStart]->m_location, tangent,
			goalLocation, rotationMatrix);

		Vector3 desiredDirection = tangent;
		desiredDirection.normalize();
		desiredDirection = m_rotationMatrix90DegreesCW.TransformDirection(desiredDirection);
		if(directionToRotate == Rotate_CCW)
		{
			desiredDirection *= -1.f;
		}
		//////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		//Adds nodes until the semicircle is built

		float currentDotProduct = -1.f;
		float previousDotProduct = -1.f;
		bool continueTurning = true;

		NodeInformation nextNodeInformation;

		int indexOfNewNode = indexOfQuarterCircleStart + 1;
		int sizeOfNodeHolder = m_tempNodes.size();
		while(continueTurning)
		{
			//get the location of the next fragment rotated towards the desired direction
			if( GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(desiredDirection, rotationMatrix,
				m_maxTurnAngleDotProductForRoadSegments, indexOfNewNode - 1, nextNodeInformation) )
			{
				//check if the new node is facing more towards the desired direction
				currentDotProduct = desiredDirection.DotProduct(nextNodeInformation.direction);
				if(currentDotProduct > previousDotProduct)
				{
					previousDotProduct = currentDotProduct;
				}
				else
				{
					continueTurning = false;
					continue;
				}

				//check if we need to add a new RoadNode
				AddNewTempNode(indexOfNewNode);

				m_tempNodes[indexOfNewNode]->SetLocation(nextNodeInformation.location);
				++indexOfNewNode;
			}
			else
			{
				continueTurning = false;
			}
		}
		/////////////////////////////////////////

		return indexOfNewNode - (indexOfQuarterCircleStart + 1);
	}

	int Application::AddSemiCircle(int indexOfSemiCircleStart, const Vector3& directionOfStartNode, const Vector3& goalLocation)
	{
		Vector3 tangent = directionOfStartNode;
		//we need at least two nodes in order to figure out our current direction
		if( tangent == Vector3(0.f, 0.f, 0.f) )
		{
			return 0;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////
		//The following calculates the direction we should rotate the fragments when building the semicircle
		Matrix4X4 rotationMatrix;
		GetBestSuitedRotationMatrix(m_tempNodes[indexOfSemiCircleStart]->m_location, tangent, goalLocation, rotationMatrix);
		Vector3 desiredDirection = tangent;
		desiredDirection.normalize();

		desiredDirection = m_rotationMatrix90DegreesCW.TransformDirection(desiredDirection);
		desiredDirection = m_rotationMatrix90DegreesCW.TransformDirection(desiredDirection);
		//////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		//Adds nodes until the semicircle is built

		float currentDotProduct = -1.f;
		float previousDotProduct = -1.f;
		bool continueTurning = true;

		NodeInformation nextNodeInformation;

		int indexOfNewNode = indexOfSemiCircleStart + 1;
		int sizeOfNodeHolder = m_tempNodes.size();
		while(continueTurning)
		{
			//get the location of the next fragment rotated towards the desired direction
			if( GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection( desiredDirection, rotationMatrix,
				m_maxTurnAngleDotProductForRoadSegments, indexOfNewNode - 1, nextNodeInformation) )
			{
				//check if the new node is facing more towards the desired direction
				currentDotProduct = desiredDirection.DotProduct(nextNodeInformation.direction);
				if(currentDotProduct > previousDotProduct)
				{
					previousDotProduct = currentDotProduct;
				}
				else
				{
					continueTurning = false;
					continue;
				}

				//check if we need to add a new RoadNode
				AddNewTempNode(indexOfNewNode);

				m_tempNodes[indexOfNewNode]->SetLocation(nextNodeInformation.location);
				++indexOfNewNode;
			}
			else
			{
				continueTurning = false;
			}
		}
		/////////////////////////////////////////

		return indexOfNewNode - (indexOfSemiCircleStart + 1);
	}

	int Application::AddNodesToFaceSpecifiedLocation(const Vector3& locationToFace, const Vector3& startingLocation,
		const Vector3& startingDirection, std::vector< RoadNode* >& nodeContainer, int indexOfNodeToPlace)
	{
		int originalIndexOfNodeToPlace = indexOfNodeToPlace;
		Vector3 distanceVector = locationToFace - startingLocation;
		distanceVector.normalize();

		Vector3 distanceVectorRotated90CW = m_rotationMatrix90DegreesCW.TransformDirection(distanceVector);

		Matrix4X4 rotationMatrix = m_roadMaxCWRotationTransformationMatrix;
		if(startingDirection.DotProduct(distanceVectorRotated90CW) > 0.f)
		{
			rotationMatrix = m_roadMaxCCWRotationTransformationMatrix;
		}

		Vector3 currentDirection = startingDirection;
		Vector3 currentLocation = startingLocation;

		int sizeOfNodeHolder = nodeContainer.size();

		bool keepIterating = true;
		float previousDotProduct = -1;
		while(keepIterating)
		{
			distanceVector = locationToFace - currentLocation;
			distanceVector.normalize();

			float currentDotProduct = currentDirection.DotProduct(distanceVector);
			if(currentDotProduct >= m_maxTurnAngleDotProductForRoadSegments)
			{
				//we can face the desired direction without rotating max degrees
				currentDirection = distanceVector;
				keepIterating = false;
			}
			else if(previousDotProduct > currentDotProduct)
			{
				keepIterating = false;
				continue;
			}
			else
			{
				currentDirection = rotationMatrix.TransformDirection(currentDirection);
			}

			if(indexOfNodeToPlace >= sizeOfNodeHolder)
			{
				nodeContainer.push_back(new RoadNode());
			}

			nodeContainer[indexOfNodeToPlace]->m_location = currentLocation + (currentDirection * m_lengthOfFragment);
			currentLocation = nodeContainer[indexOfNodeToPlace]->m_location;
			++indexOfNodeToPlace;
			previousDotProduct = currentDotProduct;
		}

		return indexOfNodeToPlace - originalIndexOfNodeToPlace;
	}

	int Application::AddNodesToFaceSpecifiedDirection(const Vector3& directionToFace, const Vector3& startingDirection,
		const Vector3& startingLocation, std::vector< RoadNode* >& nodeContainer, RotationDirection directionToRotate, int indexOfNodeToPlace)
	{
		int originalIndexOfNodeToPlace = indexOfNodeToPlace;

		Matrix4X4 rotationMatrix;
		if(directionToRotate == Rotate_CW)
		{
			rotationMatrix = Matrix4X4::RotateZDegreesMatrix(-m_maxYawRotationDegreesForRoadSegments);
		}
		else
		{
			rotationMatrix = Matrix4X4::RotateZDegreesMatrix(m_maxYawRotationDegreesForRoadSegments);
		}

		Vector3 currentLocation = startingLocation;
		Vector3 currentDirection = startingDirection;
		currentDirection.normalize();
		Vector3 desiredDirection = directionToFace;
		desiredDirection.normalize();
		float previousDotProduct = -1.f;
		float currentDotProduct = -1.f;
		bool keepIterating = true;
		int sizeOfNodeHolder = nodeContainer.size();
		while(keepIterating)
		{
			currentDotProduct = currentDirection.DotProduct(desiredDirection);
			if(currentDotProduct >= m_maxTurnAngleDotProductForRoadSegments)
			{
				//we can face the desired direction without rotating max degrees
				currentDirection = desiredDirection;
				keepIterating = false;
			}
			else if(currentDotProduct < previousDotProduct)
			{
				keepIterating = false;
				continue;
			}
			else
			{
				currentDirection = rotationMatrix.TransformDirection(currentDirection);
			}

			if(indexOfNodeToPlace >= sizeOfNodeHolder)
			{
				nodeContainer.push_back(new RoadNode());
			}

			nodeContainer[indexOfNodeToPlace]->m_location = currentLocation + (currentDirection * m_lengthOfFragment);
			currentLocation = nodeContainer[indexOfNodeToPlace]->m_location;
			++indexOfNodeToPlace;

			previousDotProduct = currentDotProduct;
		}

		return indexOfNodeToPlace - originalIndexOfNodeToPlace;
	}

	RotationDirection Application::GetBestWayToRotateToFaceLocation(const Vector3& startLocation, const Vector3& startDirection,
		const Vector3& endLocation)
	{
		RotationDirection directionToRotate = Rotate_CW;
		Vector3 distanceVector = endLocation - startLocation;

		Vector3 distanceVectorRotated90CW = m_rotationMatrix90DegreesCW.TransformDirection(distanceVector);
		if(startDirection.DotProduct(distanceVectorRotated90CW) > 0.f)
		{
			directionToRotate = Rotate_CCW;
		}

		return directionToRotate;
	}

	RotationDirection Application::GetBestSuitedRotationMatrix(const Vector3& startLocation, const Vector3& startDirection,
		const Vector3& endLocation, Matrix4X4& out_bestSuitedRotationMatrix)
	{
		RotationDirection directionToRotate = GetBestWayToRotateToFaceLocation(startLocation, startDirection, endLocation);

		switch(directionToRotate)
		{
		case Rotate_CCW:
			out_bestSuitedRotationMatrix = m_roadMaxCCWRotationTransformationMatrix;
			break;
		default:
			out_bestSuitedRotationMatrix = m_roadMaxCWRotationTransformationMatrix;
		}

		return directionToRotate;
	}

	void Application::AddDebugNodesToIndicateMerge(RoadNode* startingNode, RoadNode* nodeToMergeTo)
	{
		Vector3 distanceVector = nodeToMergeTo->m_location - startingNode->m_location;
		float distanceBetweenPoints = distanceVector.calculateRadialDistance();

		Vector3 startTangent = startingNode->GetTangentOfNode();
		Vector3 endTangent = nodeToMergeTo->GetTangentOfNode();

		startTangent.normalize();
		endTangent.normalize();

		startTangent *= distanceBetweenPoints;
		endTangent *= distanceBetweenPoints;
		startTangent.z = 0.f;
		endTangent.z = 0.f;

		int totalSegments = 50;
		float oneOverNumberOfSegments = 1.f / totalSegments;
		for (int i = 0; i < totalSegments; ++i)
		{
			float t =  i * oneOverNumberOfSegments;    // scale s to go from 0 to 1
			float s = 1 - t;

			float h1 =  (s * s) * ( 1.f + ( 2.f * t ) );											// calculate basis function 1
			float h2 = (t * t) * ( 1.f + ( 2.f * s ) );												// calculate basis function 2
			float h3 = (s * s) * t;																	// calculate basis function 3
			float h4 = -(t * t) * s;																// calculate basis function 4
			Vector3 p = (h1 * startingNode->m_location) +											// multiply and sum all funtions
				(h2 * nodeToMergeTo->m_location) +													// together to build the interpolated
				h3*startTangent +																	// point along the curve.
				h4*endTangent;

			m_debugNodesToRender.push_back(new RoadNode());
			m_debugNodesToRender.back()->m_location = p;
		}
	}

	int Application::CalculateIntersectionConnection(const Vector3& mouseWorldPos)
	{
		int indexOfTempNodeToPlace = m_indexOfLastPermanentNode + 1;

		//find the tangent of the current node
		Vector3 startTangent = m_tempNodes[m_indexOfLastPermanentNode]->GetTangentOfNode();
		if(startTangent == Vector3())
		{
			startTangent = mouseWorldPos - m_tempNodes[m_indexOfLastPermanentNode]->m_location;
		}

		//tangent of the node we are attempting to merge into
		Vector3 endTangent;
		endTangent = m_roadNodeInRange->GetTangentOfNode();

		startTangent.normalize();
		endTangent.normalize();

		Vector3 endDirection = endTangent;
		Vector3 startDirection = startTangent;
		endDirection.z = 0.f;
		startDirection.z = 0.f;

		//NOTE: The following is just to display the temporary curve created by the hermite spline equation
		//use the distance as the multiplier
		AddDebugNodesToIndicateMerge(m_tempNodes[m_indexOfLastPermanentNode], m_roadNodeInRange);

		//try to get the road to face the direction of the new node
		//figure out what direction to bend
		//CW or CCW
		std::vector< RoadNode* > localNodeContainer;
		AddNodesToFaceSpecifiedLocation(m_roadNodeInRange->m_location, m_tempNodes[m_indexOfLastPermanentNode]->m_location,
			startDirection, localNodeContainer);

		Vector3 endLocation = m_tempNodes[m_indexOfLastPermanentNode]->m_location;
		int nodeContainerSize = localNodeContainer.size();

		Vector3 newDesiredDirection = startDirection;
		if(nodeContainerSize > 1)
		{
			endLocation = localNodeContainer[nodeContainerSize - 1]->m_location;

			newDesiredDirection = localNodeContainer[nodeContainerSize - 1]->m_location 
				- localNodeContainer[nodeContainerSize - 2]->m_location;
		}
		else if(nodeContainerSize > 0)
		{
			endLocation = localNodeContainer[0]->m_location;

			newDesiredDirection = localNodeContainer[0]->m_location - m_tempNodes[m_indexOfLastPermanentNode]->m_location;
		}
		newDesiredDirection *= -1.f;

		Vector3 newStartDirection = -endDirection;
		RotationDirection directionToRotate = GetBestWayToRotateToFaceLocation(m_roadNodeInRange->m_location, 
			newStartDirection, endLocation);

		//delete the nodes and empty the array of nodes created temporarily
		for(int nodeIndex = 0; nodeIndex < nodeContainerSize; ++nodeIndex)
		{
			delete localNodeContainer[nodeIndex];
		}
		localNodeContainer.clear();

		AddNodesToFaceSpecifiedDirection(newDesiredDirection, newStartDirection, m_roadNodeInRange->m_location,
			localNodeContainer, directionToRotate);

		Vector3 currentEndDirection = -endTangent;
		endLocation = m_roadNodeInRange->m_location;
		nodeContainerSize = localNodeContainer.size();
		if(nodeContainerSize > 1)
		{
			endLocation = localNodeContainer[nodeContainerSize - 1]->m_location;

			currentEndDirection = endLocation - localNodeContainer[nodeContainerSize - 2]->m_location;
		}
		else if(nodeContainerSize > 0)
		{
			endLocation = localNodeContainer[0]->m_location;

			currentEndDirection = endLocation - m_roadNodeInRange->m_location;
		}
		currentEndDirection.normalize();

		Vector3 currentStartDirection = startDirection;
		std::vector <RoadNode* > tempStartNodes;


		Vector3 currentDesiredEndDirection;
		Vector3 currentDesiredEndDirectionRotated90CW;
		Matrix4X4 currentEndDirectionRotationMatrix;
		Vector3 currentDesiredStartDirection;
		Vector3 currentDesiredStartDirectionRotated90CW;
		Matrix4X4 currentStartDirectionRotationMatrix;
		Vector3 currentStartLocation = m_tempNodes[m_indexOfLastPermanentNode]->m_location;
		Vector3 currentEndLocation = endLocation;
		bool keepIterating = true;
		int maxIterations = 100;		//There needs to be a better way to identify cases where one a connection cannot be made

		Matrix4X4 clockwise90DegreeRotation = Matrix4X4::RotateZDegreesMatrix(-90.f);
		Matrix4X4 clockwiseMaxRotation = Matrix4X4::RotateZDegreesMatrix(-m_maxYawRotationDegreesForRoadSegments);
		Matrix4X4 counterClockwiseMaxRotation = Matrix4X4::RotateZDegreesMatrix(m_maxYawRotationDegreesForRoadSegments);

		while(keepIterating 
			&& maxIterations > 0)
		{
			currentDesiredEndDirection =  currentStartLocation - currentEndLocation;
			currentDesiredEndDirection.normalize();
			currentDesiredEndDirectionRotated90CW = clockwise90DegreeRotation.TransformDirection(currentDesiredEndDirection);

			currentEndDirectionRotationMatrix = clockwiseMaxRotation;
			if(currentEndDirection.DotProduct(currentDesiredEndDirectionRotated90CW) > 0.f)
			{
				currentEndDirectionRotationMatrix = counterClockwiseMaxRotation;
			}

			currentDesiredStartDirection = -currentDesiredEndDirection;
			currentDesiredStartDirectionRotated90CW = clockwise90DegreeRotation.TransformDirection(currentDesiredStartDirection);

			currentStartDirectionRotationMatrix = clockwiseMaxRotation;
			if(currentStartDirection.DotProduct(currentDesiredStartDirectionRotated90CW) > 0.f)
			{
				currentStartDirectionRotationMatrix = counterClockwiseMaxRotation;
			}

			int nodesAdded = 0;
			//start with the end location node
			if(currentEndDirection.DotProduct(currentDesiredEndDirection) < m_maxTurnAngleDotProductForRoadSegments)
			{
				currentEndDirection = currentEndDirectionRotationMatrix.TransformDirection(currentEndDirection);
				//add a new segment rotated towards the final direction
				localNodeContainer.push_back(new RoadNode());
				localNodeContainer.back()->m_location = currentEndLocation + (currentEndDirection * m_lengthOfFragment);
				currentEndLocation = localNodeContainer.back()->m_location;

				++nodesAdded;
			}

			if(currentStartDirection.DotProduct(currentDesiredStartDirection) < m_maxTurnAngleDotProductForRoadSegments)
			{
				currentStartDirection = currentStartDirectionRotationMatrix.TransformDirection(currentStartDirection);
				//add a new segment rotated towards the final direction
				tempStartNodes.push_back(new RoadNode());
				tempStartNodes.back()->m_location = currentStartLocation + (currentStartDirection * m_lengthOfFragment);
				currentStartLocation = tempStartNodes.back()->m_location;

				++nodesAdded;
			}

			if(nodesAdded == 0)
			{
				break;
			}

			--maxIterations;
		}

		if(maxIterations <= 0)
		{
			for(int i = 0; i < localNodeContainer.size(); ++i)
			{
				delete localNodeContainer[i];
			}

			for(int i = 0; i < tempStartNodes.size(); ++i)
			{
				delete tempStartNodes[i];
			}
		}
		else
		{
			int sizeOfNodeHolder = m_tempNodes.size();
			for(int i = 0; i < tempStartNodes.size(); ++i)
			{
				AddNewTempNode(indexOfTempNodeToPlace);

				m_tempNodes[indexOfTempNodeToPlace]->m_location = tempStartNodes[i]->m_location;
				++indexOfTempNodeToPlace;
			}

			for(int i = localNodeContainer.size() - 1; i > -1; --i)
			{
				AddNewTempNode(indexOfTempNodeToPlace);

				m_tempNodes[indexOfTempNodeToPlace]->m_location = localNodeContainer[i]->m_location;
				++indexOfTempNodeToPlace;
			}

			//delete all the nodes that were created temporarily
			for(int i = 0; i < localNodeContainer.size(); ++i)
			{
				delete localNodeContainer[i];
			}

			for(int i = 0; i < tempStartNodes.size(); ++i)
			{
				delete tempStartNodes[i];
			}

			//RemoveConnection from last node if needed
			if(m_tempNodes.size() > indexOfTempNodeToPlace)
			{
				m_tempNodes[indexOfTempNodeToPlace - 1]->RemoveNextNode(m_tempNodes[indexOfTempNodeToPlace]);
			}
		}

		//return the number of nodes that were added
		int nodesAdded = indexOfTempNodeToPlace - (m_indexOfLastPermanentNode + 1);
		return nodesAdded;
	}
	
	int Application::CreateDirectConnection(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode)
	{
		Vector3 startLocation = startNode->m_location;
		Vector3 distanceVector = goalLocation - startLocation;
		Vector3 normalizedDirectionCurrentNodeToGoal = distanceVector;
		normalizedDirectionCurrentNodeToGoal.normalize();
		Vector3 locationOfNextNode = startLocation;
		float distanceSquaredFromLastNode = distanceVector.calculateRadialDistanceSquared();
		int indexOfTempNodeToPlace = indexOfNextTempNode;

		while(distanceSquaredFromLastNode > m_lengthOfFragmentSquared)
		{
			locationOfNextNode = startLocation + (normalizedDirectionCurrentNodeToGoal * m_lengthOfFragment);

			AddNewTempNode(indexOfTempNodeToPlace);
			m_tempNodes[indexOfTempNodeToPlace]->SetLocation(locationOfNextNode);

			//recalculate distance to goal location
			distanceVector = goalLocation - locationOfNextNode;
			distanceSquaredFromLastNode = distanceVector.calculateRadialDistanceSquared();

			startLocation = locationOfNextNode;
			++indexOfTempNodeToPlace;
		}

		return indexOfTempNodeToPlace - indexOfNextTempNode;
	}

	int Application::ConstructRoadTowardsSpecifiedLocation(const Vector3& goalLocation, RoadNode* startNode, int indexOfNextTempNode)
	{
		RoadNode* currentNode = startNode;
		Vector3 distanceVector = goalLocation - currentNode->m_location;
		float distanceSquared = distanceVector.calculateRadialDistanceSquared();
		Vector3 normalizedDirectionCurrentNodeToGoal = distanceVector;
		normalizedDirectionCurrentNodeToGoal.normalize();

		int indexOfTempNodeToPlace = indexOfNextTempNode;
		Vector3 normalizedDirectionOfLastSegment = currentNode->GetTangentOfNode();
		normalizedDirectionOfLastSegment.normalize();
		Vector3 normalizedDirectionOfLastSegmentRotated90DegreesCW = 
			m_rotationMatrix90DegreesCW.TransformDirection(normalizedDirectionOfLastSegment);
		NodeInformation nextNodeInformation;

		//check if we need to add a quarter circle first
		Matrix4X4 rotationMatrix = m_roadMaxCCWRotationTransformationMatrix;
		Vector3 directionToTopOfSemicircle = -normalizedDirectionOfLastSegmentRotated90DegreesCW;

		float dotProductLastSegment90CWAndDirectionCurrentNodeToGoal = 
			normalizedDirectionOfLastSegmentRotated90DegreesCW.DotProduct(normalizedDirectionCurrentNodeToGoal);
		
		if(dotProductLastSegment90CWAndDirectionCurrentNodeToGoal > 0.f )
		{
			rotationMatrix = m_roadMaxCWRotationTransformationMatrix;
			directionToTopOfSemicircle = normalizedDirectionOfLastSegmentRotated90DegreesCW;
		}

		Vector3 topOfSemicircle = currentNode->m_location + (directionToTopOfSemicircle * m_radiusOfRoadCircle * 2.f);
		Vector3 middleOfSemicircle = currentNode->m_location + (directionToTopOfSemicircle * m_radiusOfRoadCircle);
		Vector3 closestEdgeOfSemicircle = currentNode->m_location + (normalizedDirectionOfLastSegment * m_radiusOfRoadCircle);
		Vector3 farEdgeOfSemicircle = currentNode->m_location + (-normalizedDirectionOfLastSegment * m_radiusOfRoadCircle);

		Vector3 distanceCenterToGoal = goalLocation - middleOfSemicircle;
		Vector3 distanceTopToGoal = goalLocation - topOfSemicircle;
		Vector3 distanceClosestEdgeOfSemicircleToGoal = goalLocation - closestEdgeOfSemicircle;
		Vector3 distanceFarEdgeOfSemicircleToGoal = goalLocation - farEdgeOfSemicircle;

		int nodesAdded = 0;

		if(distanceTopToGoal.DotProduct(directionToTopOfSemicircle) < 0.f)
		{
			//we are below the top of the circle
			if(distanceCenterToGoal.DotProduct(directionToTopOfSemicircle) < 0.f)
			{
				//we are below the half section of the semicircle
				if(distanceCenterToGoal.DotProduct(normalizedDirectionOfLastSegment) < 0.f)
				{
					//we are going the opposite way of the current road segment

					nodesAdded += AddSemiCircle(indexOfTempNodeToPlace - 1 + nodesAdded,
						m_tempNodes[indexOfTempNodeToPlace - 1 + nodesAdded]->GetTangentOfNode(), goalLocation);

					if(distanceFarEdgeOfSemicircleToGoal.DotProduct(-normalizedDirectionOfLastSegment) < 0.f)
					{	
						nodesAdded += AddQuarterCircle(indexOfTempNodeToPlace - 1 + nodesAdded, 
							m_tempNodes[indexOfTempNodeToPlace - 1 + nodesAdded]->GetTangentOfNode(), goalLocation);
					}
				}
			}
			else
			{
				//we are above the half section of the semicircle
				if(distanceCenterToGoal.DotProduct(normalizedDirectionOfLastSegment) < 0.f)
				{
					//we are going the opposite way of the current road segment
					nodesAdded += AddSemiCircle(indexOfTempNodeToPlace - 1 + nodesAdded,
						m_tempNodes[indexOfTempNodeToPlace - 1 + nodesAdded]->GetTangentOfNode(), goalLocation);
				}
				else
				{
					//we are going on the same direction as the last fragment
					if(distanceClosestEdgeOfSemicircleToGoal.DotProduct(normalizedDirectionOfLastSegment) < 0.f)
					{
						//we have not gone past the closest edge of the semicircle
						nodesAdded += AddQuarterCircle(indexOfTempNodeToPlace - 1 + nodesAdded,
							m_tempNodes[indexOfTempNodeToPlace - 1 + nodesAdded]->GetTangentOfNode(), goalLocation);
					}
				}
			}
		}
		else
		{
			//we are above the top of the circle
			if(distanceCenterToGoal.DotProduct(normalizedDirectionOfLastSegment) < 0.f)
			{
				//we are going opposite the direction of the current road fragment
				nodesAdded += AddQuarterCircle(indexOfTempNodeToPlace - 1 + nodesAdded,
					m_tempNodes[indexOfTempNodeToPlace - 1 + nodesAdded]->GetTangentOfNode(), goalLocation);
			}
			else
			{
				//we are going along the same direction of the previous road
				if(distanceClosestEdgeOfSemicircleToGoal.DotProduct(normalizedDirectionOfLastSegment) < 0.f)
				{
					//we have not gone past the closest edge of the semicircle
					nodesAdded += AddQuarterCircle(indexOfTempNodeToPlace - 1 + nodesAdded,
						m_tempNodes[indexOfTempNodeToPlace - 1 + nodesAdded]->GetTangentOfNode(), goalLocation);
				}
			}
		}

		if(nodesAdded > 0)
		{
			//lets modify the previous values
			indexOfTempNodeToPlace += nodesAdded;
			currentNode = m_tempNodes[indexOfTempNodeToPlace - 1];
		}
		
		while(distanceSquared > m_lengthOfFragmentSquared)
		{
			distanceVector = goalLocation - currentNode->m_location;
			normalizedDirectionCurrentNodeToGoal = distanceVector;
			normalizedDirectionCurrentNodeToGoal.normalize();
			distanceSquared = distanceVector.calculateRadialDistanceSquared();

			//calculate the direction of the last segment
			normalizedDirectionOfLastSegment = m_tempNodes[indexOfTempNodeToPlace - 1]->GetTangentOfNode();
			normalizedDirectionOfLastSegment.normalize();

			if(normalizedDirectionOfLastSegment == Vector3())
			{
				int x = 1;
			}

			normalizedDirectionOfLastSegmentRotated90DegreesCW = 
				m_rotationMatrix90DegreesCW.TransformDirection(normalizedDirectionOfLastSegment);

			//check if we are rotating clockwise or counterclockwise
			dotProductLastSegment90CWAndDirectionCurrentNodeToGoal
				= normalizedDirectionOfLastSegmentRotated90DegreesCW.DotProduct(normalizedDirectionCurrentNodeToGoal);

			Matrix4X4 rotationMatrix = m_roadMaxCCWRotationTransformationMatrix;
			if(dotProductLastSegment90CWAndDirectionCurrentNodeToGoal > 0.f )
			{
				rotationMatrix = m_roadMaxCWRotationTransformationMatrix;
			}

			//get the location of the next fragment rotated towards the desired direction
			if(GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(normalizedDirectionCurrentNodeToGoal,
				rotationMatrix, m_maxTurnAngleDotProductForRoadSegments, indexOfTempNodeToPlace - 1, nextNodeInformation))
			{
				//check our stopping conditions
				float newDistanceSquared = (nextNodeInformation.location - goalLocation).calculateRadialDistanceSquared();

				//we are getting farther away from the mouse
				if( newDistanceSquared > distanceSquared
					|| newDistanceSquared <= m_lengthOfFragmentSquared )
				{
					break;
				}

				//add another node
				AddNewTempNode(indexOfTempNodeToPlace);

				m_tempNodes[indexOfTempNodeToPlace]->SetLocation(nextNodeInformation.location);
				currentNode = m_tempNodes[indexOfTempNodeToPlace];
			}
			else
			{
				break;
			}

			++indexOfTempNodeToPlace;
		}

		return indexOfTempNodeToPlace - indexOfNextTempNode;
	}

	int Application::ConstructTurnTowardsSpecifiedLocation( RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode )
	{
		RoadNode* currentNode = startNode;
		Vector3 distanceVector = goalLocation - currentNode->m_location;
		Vector3 normalizedDirectionCurrentNodeToGoal = distanceVector;
		normalizedDirectionCurrentNodeToGoal.normalize();
		int indexOfTempNodeToPlace = indexOfNextTempNode;

		Vector3 normalizedDirectionOfLastSegment = startNode->GetTangentOfNode();
		normalizedDirectionOfLastSegment.normalize();

		Vector3 normalizedDirectionOfLastSegmentRotated90DegreesCW 
			= m_rotationMatrix90DegreesCW.TransformDirection(normalizedDirectionOfLastSegment);

		//check if we are rotating clockwise or counterclockwise
		float dotProductLastSegment90CWAndDirectionCurrentNodeToGoal
			= normalizedDirectionOfLastSegmentRotated90DegreesCW.DotProduct(normalizedDirectionCurrentNodeToGoal);

		//calculate the variables needed to know where the top of the semicircle and the direction to it
		Matrix4X4 rotationMatrix = m_roadMaxCCWRotationTransformationMatrix;
		Vector3 directionToTopOfSemicircle = -normalizedDirectionOfLastSegmentRotated90DegreesCW;
		if(dotProductLastSegment90CWAndDirectionCurrentNodeToGoal > 0.f )
		{
			rotationMatrix = m_roadMaxCWRotationTransformationMatrix;
			directionToTopOfSemicircle = normalizedDirectionOfLastSegmentRotated90DegreesCW;
		}
		Vector3 topOfSemicircle = currentNode->m_location + (directionToTopOfSemicircle * m_radiusOfRoadCircle * 2.f);
		Vector3 middleOfSemicircle = currentNode->m_location + (directionToTopOfSemicircle * m_radiusOfRoadCircle);

		int nodesAdded = 0;
		//figure out what zone the mouse is located at
		Vector3 distanceCenterToGoal = goalLocation - middleOfSemicircle;
		if( distanceCenterToGoal.DotProduct(directionToTopOfSemicircle) > 0.f )
		{
			//we are on the top regions
			//check if we are aligned or not to the direction of the last segment
			if( normalizedDirectionOfLastSegment.DotProduct(distanceCenterToGoal) > 0.f )
			{
				//we are aligned with the direction and on the top zone
				//this means that we spawn at least a quarter circle
				nodesAdded = AddQuarterCircle(indexOfTempNodeToPlace - 1, normalizedDirectionOfLastSegment, goalLocation);
			}
			else
			{
				//we are facing the opposite direction and on the top zone
				//this means we at least spawn a semicircle
				nodesAdded = AddSemiCircle(indexOfTempNodeToPlace - 1, normalizedDirectionOfLastSegment, goalLocation);
			}
		}
		else
		{
			//we are on the bottom regions
			if( normalizedDirectionOfLastSegment.DotProduct(normalizedDirectionCurrentNodeToGoal) < 0.f )
			{
				nodesAdded = AddSemiCircle(indexOfTempNodeToPlace - 1, normalizedDirectionOfLastSegment, goalLocation);

				if(nodesAdded > 0)
				{
					indexOfTempNodeToPlace += nodesAdded;
					normalizedDirectionOfLastSegment = m_tempNodes[indexOfTempNodeToPlace - 1]->GetTangentOfNode();
					normalizedDirectionOfLastSegment.normalize();

					nodesAdded = AddQuarterCircle(indexOfTempNodeToPlace - 1, normalizedDirectionOfLastSegment, goalLocation);
					indexOfTempNodeToPlace += nodesAdded;
				}
			}
		}

		return indexOfTempNodeToPlace - indexOfNextTempNode;
	}

	int Application::ConstructBestPossibleRoad(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode)
	{
		int indexOfTempNodeToPlace = indexOfNextTempNode;
		RoadNode* currentNode = startNode;
		Vector3 distanceVector = goalLocation - currentNode->m_location;
		Vector3 normalizedDirectionCurrentNodeToGoal = distanceVector;
		normalizedDirectionCurrentNodeToGoal.normalize();

		//calculate the direction of the last segment
		Vector3 normalizedDirectionOfLastSegment = m_tempNodes[indexOfTempNodeToPlace - 1]->GetTangentOfNode();
		normalizedDirectionOfLastSegment.normalize();

		Vector3 normalizedDirectionOfLastSegmentRotated90DegreesCW = 
			m_rotationMatrix90DegreesCW.TransformDirection(normalizedDirectionOfLastSegment);

		//Get the midpoints of the road circles that can be constructed from this node
		Vector3 midpoint1 = currentNode->m_location + (normalizedDirectionOfLastSegmentRotated90DegreesCW * m_radiusOfRoadCircle);
		Vector3 midpoint2 = currentNode->m_location - (normalizedDirectionOfLastSegmentRotated90DegreesCW * m_radiusOfRoadCircle);

		//check if the mouse is outside both of the circles that can be constructed from the last road segment
		float distanceSquaredFromMidpoint1 = (goalLocation - midpoint1).calculateRadialDistanceSquared();
		float distanceSquaredFromMidpoint2 = (goalLocation - midpoint2).calculateRadialDistanceSquared();
		float roadCircleRadiusSquared = m_radiusOfRoadCircle * m_radiusOfRoadCircle;

		int nodesAdded = 0;
		//Check if the mouse is not inside the road circle that can be spawned
		if(distanceSquaredFromMidpoint1 >= roadCircleRadiusSquared
			&& distanceSquaredFromMidpoint2 >= roadCircleRadiusSquared)
		{
			//the mouse cursor can be directly reached
			//keep adding nodes if the distance is greater than the size of one segment
			nodesAdded = ConstructRoadTowardsSpecifiedLocation(goalLocation, currentNode, indexOfTempNodeToPlace);
		}
		else
		{
			//nodesAdded = ConstructTurnTowardsSpecifiedLocation(currentNode, goalLocation, indexOfTempNodeToPlace);
		}

		return nodesAdded;
	}

	int Application::BranchRoadFromSpecifiedNodeTowardsGoalLocation(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode)
	{
		int indexOfTempNodeToPlace = indexOfNextTempNode;
		RoadNode* currentNode = startNode;

		Vector3 startLocation = currentNode->m_location;
		Vector3 distanceVector = goalLocation - startLocation;
		Vector3 normalizedDirectionCurrentNodeToGoal = distanceVector;
		normalizedDirectionCurrentNodeToGoal.normalize();

		std::vector<Vector3> directionOfBranchingSegments;
		m_roadNodeIntersectionToForkFrom->GetDirectionOfBranchingSegments(directionOfBranchingSegments);

		for(int i = 0; i < directionOfBranchingSegments.size(); ++i)
		{
			directionOfBranchingSegments[i].normalize();
		}

		//We have 3 options
		Vector3 defaultDirectionOfIntersection = m_roadNodeIntersectionToForkFrom->GetDefaultDirectionOfIntersection();
		defaultDirectionOfIntersection.normalize();
		Vector3 clockwiseRotatedDirection = m_roadMaxCWRotationTransformationMatrix.TransformDirection(defaultDirectionOfIntersection);
		Vector3 counterClockwiseRotatedDirection = m_roadMaxCCWRotationTransformationMatrix.TransformDirection(defaultDirectionOfIntersection);

		//see which of the directions is closest to the one we are facing
		Vector3 closestDirection = defaultDirectionOfIntersection;
		float dotProductOfClosestDirection = defaultDirectionOfIntersection.DotProduct(normalizedDirectionCurrentNodeToGoal);
		float currentDotProduct = dotProductOfClosestDirection;

		currentDotProduct = clockwiseRotatedDirection.DotProduct(normalizedDirectionCurrentNodeToGoal);
		if(currentDotProduct > dotProductOfClosestDirection)
		{
			closestDirection = clockwiseRotatedDirection;
			dotProductOfClosestDirection = currentDotProduct;
		}

		currentDotProduct = counterClockwiseRotatedDirection.DotProduct(normalizedDirectionCurrentNodeToGoal);
		if(currentDotProduct > dotProductOfClosestDirection)
		{
			closestDirection = counterClockwiseRotatedDirection;
			dotProductOfClosestDirection = currentDotProduct;
		}

		//This adds the first node in the intended direction
		if(normalizedDirectionCurrentNodeToGoal.DotProduct(defaultDirectionOfIntersection) < 0.f
			|| distanceVector.calculateRadialDistanceSquared() > m_lengthOfFragmentSquared)
		{
			//at the moment add a temporary node towards the closest direction
			AddNewTempNode(indexOfTempNodeToPlace);

			m_tempNodes[indexOfTempNodeToPlace]->SetLocation(m_roadNodeIntersectionToForkFrom->m_location + (closestDirection * m_lengthOfFragment));
			currentNode = m_tempNodes[indexOfTempNodeToPlace];
			++indexOfTempNodeToPlace;
		}
		else
		{
			return 0;
		}

		int nodesAdded = ConstructBestPossibleRoad(currentNode, goalLocation, indexOfTempNodeToPlace);
		return nodesAdded;
	}

	int Application::ConstructRoadFromNodeWithIndex0(RoadNode* startNode, const Vector3& goalLocation, int indexOfNextTempNode)
	{
		int nodesAdded = 0;
		if(m_roadNodeIntersectionToForkFrom == nullptr)
		{
			nodesAdded = CreateDirectConnection(startNode, goalLocation, indexOfNextTempNode);
		}
		else
		{
			nodesAdded = BranchRoadFromSpecifiedNodeTowardsGoalLocation(startNode, goalLocation, indexOfNextTempNode);
		}

		return nodesAdded;
	}

	void Application::CalculateSplineToMousePos()
	{
		if(!m_splineMode 
			|| m_indexOfLastPermanentNode < 0)
		{
			return;
		}

		Vector3 mouseWorldPos;
		bool mousePositionHitsDesiredElevation = GetMouseWorldPosWithSpecifiedZ(mouseWorldPos, 0.f);

		
		if( s_inputLogger->IsCharacterDown('Q') )
		{
			//Check if we need to automatically finish the road
			CheckForRoadNodesWithinRange(mouseWorldPos, m_roadNodeClusterInRange, m_roadNodeInRange, m_intersectionNodeIndex);
			if(m_roadNodeInRange == nullptr)
			{
				CheckForIntersectionNodesWithinRange(mouseWorldPos, m_intersectionNodeInRange, m_intersectionNodeIndex);
				m_roadNodeInRange = m_intersectionNodeInRange;
			}

			if(m_roadNodeInRange)
			{
				int nodesAdded = CalculateIntersectionConnection(mouseWorldPos);
				m_indexOfLastTempNode = m_indexOfLastPermanentNode + nodesAdded;
			}
			return;
		}

		if(mousePositionHitsDesiredElevation)
		{
			RoadNode* currentNode = m_tempNodes[m_indexOfLastPermanentNode];

			//calculate the distance and direction from the last node to the mouse
			Vector3 deltaDistanceCurrentNodeToMouse = mouseWorldPos - currentNode->m_location;
			deltaDistanceCurrentNodeToMouse.z = 0.f;			//make sure to zero out the z axis

			Vector3 normalizedDirectionCurrentNodeToMouse = deltaDistanceCurrentNodeToMouse;
			normalizedDirectionCurrentNodeToMouse.normalize();

			float distanceSquaredFromLastPermanentNode = deltaDistanceCurrentNodeToMouse.calculateRadialDistanceSquared();

			//variables used to recalculate the best path to the mouse cursor
			int indexOfTempNodeToPlace = m_indexOfLastPermanentNode + 1;
			int sizeOfNodeHolder = m_tempNodes.size();
			Vector3 locationOfNextNode;

			//Add the next road nodes in any available direction
			if(m_indexOfLastPermanentNode == 0)
			{
				int nodesAdded = ConstructRoadFromNodeWithIndex0(currentNode, mouseWorldPos, indexOfTempNodeToPlace);
				indexOfTempNodeToPlace += nodesAdded;
				m_indexOfLastTempNode = indexOfTempNodeToPlace - 1;
			}
			else
			{
				int nodesAdded = ConstructBestPossibleRoad(currentNode, mouseWorldPos, indexOfTempNodeToPlace);
				indexOfTempNodeToPlace += nodesAdded;
				m_indexOfLastTempNode = indexOfTempNodeToPlace - 1;
					/*//keep adding nodes if the distance is greater than the size of one segment
					NodeInformation nextNodeInformation;
					while(distanceSquared > m_lengthOfFragmentSquared)
					{
						deltaDistanceCurrentNodeToMouse = mouseWorldPos - currentNode->m_location;
						normalizedDirectionCurrentNodeToMouse = deltaDistanceCurrentNodeToMouse;
						normalizedDirectionCurrentNodeToMouse.normalize();
						distanceSquared = deltaDistanceCurrentNodeToMouse.calculateRadialDistanceSquared();

						//calculate the direction of the last segment
						normalizedDirectionOfLastSegment = m_tempNodes[indexOfTempNodeToPlace - 1]->m_location
							- m_tempNodes[indexOfTempNodeToPlace - 2]->m_location;
						normalizedDirectionOfLastSegment.normalize();

						normalizedDirectionOfLastSegmentRotated90DegreesCW = 
							clockwise90DegreeRotation.TransformDirection(normalizedDirectionOfLastSegment);

						//check if we are rotating clockwise or counterclockwise
						float dotProductLastSegment90CWAndDirectionCurrentNodeToMouse
							= normalizedDirectionOfLastSegmentRotated90DegreesCW.DotProduct(normalizedDirectionCurrentNodeToMouse);

						Matrix4X4 rotationMatrix = clockwiseMaxRotation;
						if(dotProductLastSegment90CWAndDirectionCurrentNodeToMouse < 0.f )
						{
							rotationMatrix = counterClockwiseMaxRotation;
						}

						//get the location of the next fragment rotated towards the desired direction
						if(GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(normalizedDirectionCurrentNodeToMouse,
							rotationMatrix, m_maxTurnAngleDotProductForRoadSegments, indexOfTempNodeToPlace - 1, nextNodeInformation))
						{
							//check our stopping conditions
							float newDistanceSquared = (nextNodeInformation.location - mouseWorldPos).calculateRadialDistanceSquared();

							//we are getting farther away from the mouse
							//ignore the condition if the fragment is the first one we are attempting to place
							if(newDistanceSquared > distanceSquared)
							{
								break;
							}

							//check if we need to add a new RoadNode
							AddNewTempNode(indexOfTempNodeToPlace);

							m_tempNodes[indexOfTempNodeToPlace]->SetLocation(nextNodeInformation.location);
							currentNode = m_tempNodes[indexOfTempNodeToPlace];
							++indexOfTempNodeToPlace;
						}
					}*/

				//m_indexOfLastTempNode = indexOfTempNodeToPlace - 1;
			}

			for(int i = m_indexOfLastPermanentNode + 1; i < indexOfTempNodeToPlace; ++i)
			{
				m_tempNodes[i]->m_isValid = false;
			}
		}

		if(m_tempNodes.size() > m_indexOfLastTempNode + 1)
		{
			m_tempNodes[m_indexOfLastTempNode]->RemoveNextNode(m_tempNodes[m_indexOfLastTempNode + 1]);
		}


	}

	void Application::AddNewTempNode( int indexOfNewNode )
	{
		RoadNode* newNode = nullptr;
		int currentIndex = m_tempNodes.size();

		if(indexOfNewNode < m_tempNodes.size())
		{
			m_tempNodes[indexOfNewNode]->m_isValid = true;
		}

		while(indexOfNewNode >= m_tempNodes.size())
		{
			newNode = new RoadNode();

			if(currentIndex != 0)
			{
				newNode->AddPreviousNode(m_tempNodes[currentIndex - 1]);

				if(currentIndex - 1 > m_indexOfLastPermanentNode)
				{
					m_tempNodes[currentIndex - 1]->AddNextNode(newNode);
				}
			}

			m_tempNodes.push_back(newNode);
		}
	}

	void Application::RenderSplines()
	{
		//check if there are temporary splines
		if(m_splineMode 
			&& !m_tempNodes.empty()
			&& g_renderTempNodes)
		{
			for(int i = m_indexOfLastPermanentNode + 1; i <= m_indexOfLastTempNode; ++i)
			{
				m_tempNodes[i]->Render(m_matrixStack, Vector3(0.f, 1.f, 0.f));
			}

			if(m_currentRoadNodeCluster)
			{
				m_currentRoadNodeCluster->Render(m_matrixStack, Vector3(1.f, 0.f, 0.f), m_showDirectionOnPlacedRoads);
			}
		}

		if(g_renderRoadNodeClusters)
		{
			for(int i = 0; i < m_roadNodeClusters.size(); ++i)
			{
				m_roadNodeClusters[i]->Render(m_matrixStack, Vector3(1.f, 0.f, 0.f), m_showDirectionOnPlacedRoads);
			}
		}

		//render the intersection nodes
		if(g_renderIntersections)
		{
			for(int i = 0; i < m_intersectionNodes.size(); ++i)
			{
				m_intersectionNodes[i]->Render(m_matrixStack);
			}
		}

		std::vector< NodeInformation > arrowheads;
		NodeInformation currentArrowHeadInformation;

		glLoadMatrixf(m_matrixStack.GetTopMatrixFV());
		glDisable(GL_TEXTURE_2D);
		glUseProgram(0);

		glBegin(GL_LINES);
		{
			glColor3f(1.f,0.f,0.f);

			Vector3 currentNodeLocation;
			Vector3 prevLocation;
			int countSinceLastArrowDisplay = 0;
			int nodesBetweenArrowRender = 5;
			for(int i = m_indexOfLastPermanentNode + 1; i <= m_indexOfLastTempNode; ++i)
			{
				currentNodeLocation = m_tempNodes[i-1]->m_location;
				prevLocation = currentNodeLocation;
				glVertex3f(currentNodeLocation.x, currentNodeLocation.y, currentNodeLocation.z);
				currentNodeLocation = m_tempNodes[i]->m_location;
				glVertex3f(currentNodeLocation.x, currentNodeLocation.y, currentNodeLocation.z);

				++countSinceLastArrowDisplay;
				if( countSinceLastArrowDisplay >= nodesBetweenArrowRender && m_showDirectionOnTempNodes)
				{
					//render the arrowhead
					Vector3 directionOfRoad = currentNodeLocation - prevLocation;
					directionOfRoad.normalize();

					currentArrowHeadInformation.location = currentNodeLocation;
					currentArrowHeadInformation.direction = directionOfRoad;
					arrowheads.push_back(currentArrowHeadInformation);

					countSinceLastArrowDisplay = 0;
				}
			}
		}
		glEnd();

		if(arrowheads.size() > 0)
		{
			glBegin(GL_TRIANGLES);
			{
				glColor3f(0.f,0.5f,0.5f);

				Vector3 point1;
				Vector3 point2;
				Vector3 point3;
				Vector3 rotatedCWDirection;
				Matrix4X4 rotationMatrix90CW = Matrix4X4::RotateZDegreesMatrix(-90.f);
				for(int i = 0; i < arrowheads.size(); ++i)
				{
					currentArrowHeadInformation = arrowheads[i];
					point1 = currentArrowHeadInformation.location;
					point2 = currentArrowHeadInformation.location - (currentArrowHeadInformation.direction * m_lengthOfFragment);
					point3 = currentArrowHeadInformation.location - (currentArrowHeadInformation.direction * m_lengthOfFragment);

					rotatedCWDirection = rotationMatrix90CW.TransformDirection(currentArrowHeadInformation.direction);
					point2 -= (rotatedCWDirection * m_lengthOfFragment * 0.5f);
					point3 += (rotatedCWDirection * m_lengthOfFragment * 0.5f);

					glVertex3f(point1.x, point1.y, point1.z);
					glVertex3f(point2.x, point2.y, point2.z);
					glVertex3f(point3.x, point3.y, point3.z);
				}
			}
			glEnd();
		}
	}

	void Application::DrawHUD()
	{
		//lets put the hud on the top left part of the window
		Vector2 windowSize = theRenderer.getWindowSize();
		AABB2 textRenderWindow( 0.f, windowSize.y - (m_HUDFontHeight + m_HUDLineBreakHeight), windowSize.x, windowSize.y);

		std::string textToPrint = "MAX BEND ANGLE = " + std::to_string((long double)m_maxYawRotationDegreesForRoadSegments);
		theRenderer.drawText( textToPrint.c_str(), "test", m_HUDFontHeight, textRenderWindow, TEXT_ALIGN_BOTTOM, toRGBA( 1.f, 1.f, 1.f ) );

		//print the strength of the end tangent
		float maxY = textRenderWindow.getMins().y;
		textRenderWindow.setMins(0.f, maxY - (m_HUDFontHeight + m_HUDLineBreakHeight));
		textRenderWindow.setMaxs(windowSize.x, maxY);
		textToPrint = "Capacity of tempNodes = " + std::to_string( (long long)m_tempNodes.capacity() );
		theRenderer.drawText( textToPrint.c_str(), "test", m_HUDFontHeight, textRenderWindow, TEXT_ALIGN_BOTTOM, toRGBA( 1.f, 1.f, 1.f ) );

		maxY = textRenderWindow.getMins().y;
		textRenderWindow.setMins(0.f, maxY - (m_HUDFontHeight + m_HUDLineBreakHeight));
		textRenderWindow.setMaxs(windowSize.x, maxY);
		if(m_currentRoadNodeCluster != nullptr)
		{
			textToPrint = "Capacity of currentRoadNodeCluster = " + std::to_string( (long long)m_currentRoadNodeCluster->m_roadNodes.size() );
		}
		else
		{
			textToPrint = "Capacity of currentRoadNodeCluster = 0"; 
		}
		theRenderer.drawText( textToPrint.c_str(), "test", m_HUDFontHeight, textRenderWindow, TEXT_ALIGN_BOTTOM, toRGBA( 1.f, 1.f, 1.f ) );
	}

	void Application::updateInput( bool continueUpdating )
	{
		m_updateInput = continueUpdating;
	}

	void Application::keyPressed( size_t charPressed )
	{
		s_inputLogger->onKeyDown( static_cast<char>(charPressed) );
	}

	void Application::KeyReleased( size_t charReleased )
	{
		s_inputLogger->onKeyUp( static_cast<char>(charReleased) );
	}

	void Application::UpdateKeyInput()
	{
		if( s_inputLogger->GetCharJustPressed() == '~' )
		{
			m_openCommandConsole = !m_openCommandConsole;
		}
		else if( m_openCommandConsole )
		{
			if( s_inputLogger->CharacterIsDown() )
			{
				theConsole.keyPressed( s_inputLogger->GetLastPressedChar() );
			}
		}
		else
		{
			if( s_inputLogger->CharacterWasJustPressed() )
			{
				switch( s_inputLogger->GetCharJustPressed() )
				{
				case 27:
					//check that the key was not previously pressed
					if(m_splineMode)
					{
						ExitSplineMode();
					}
					else
					{
						PostQuitMessage( 0 );
						m_stop = true;
					}
					break;

				case 'j':
				case 'J':
					m_maxYawRotationDegreesForRoadSegments += 1.f;
					m_maxYawRotationDegreesForRoadSegments = std::min(m_maxYawRotationDegreesForRoadSegments, 8.f);
					PrecalculateRoadVariables();
					break;

				case 'k':
				case 'K':
					m_maxYawRotationDegreesForRoadSegments -= 1.f;
					m_maxYawRotationDegreesForRoadSegments = std::max(m_maxYawRotationDegreesForRoadSegments, 1.f);
					PrecalculateRoadVariables();
					break;

				case 'o':
				case 'O':
					m_showDirectionOnTempNodes = !m_showDirectionOnTempNodes;
					break;

				case 'p':
				case 'P':
					m_showDirectionOnPlacedRoads = !m_showDirectionOnPlacedRoads;
					break;

				case 'm':
				case 'M':
					m_showSecondCurveSystem = !m_showSecondCurveSystem;
					break;

				case 'y':
				case 'Y':
					g_renderIntersections = !g_renderIntersections;
					break;

				case 'u':
				case 'U':
					g_renderRoadNodeClusters = !g_renderRoadNodeClusters;
					break;

				case 'i':
				case 'I':
					g_renderTempNodes = !g_renderTempNodes;
					break;

				default:
					break;
				}
			}
		}
	}

	void Application::aSyncKeyPressed()
	{
		bool consoleUsingKey = false;

		if( m_openCommandConsole )
		{
			consoleUsingKey = theConsole.aSyncKeyPressed();
		}
		if( !consoleUsingKey )
		{
			//free to use the key
		}
	}

	void Application::storeWindowSize( const HWND& windowHandle )
	{
		RECT clientRect;
		GetClientRect(m_hWnd, &clientRect );
		m_windowSize.x = (float)clientRect.right;
		m_windowSize.y = (float)clientRect.bottom;
	}

	void Application::initiateDrivingSystem()
	{
		initiateRoadSystem();
		m_vehicleManager = new VehicleManager( m_roadSystem, MAX_CAR_NUMBER );

		/*Rgba vehicleColor;

		float baseSpeed = 60.f;
		for( int i = 0; i < MAX_CAR_NUMBER; ++i )
		{
			m_vehicles.push_back( new Vehicle( baseSpeed + i * 15.f, Rgba( RandZeroToOne(), RandZeroToOne(), RandZeroToOne() ) ) );
			m_vehicles.back()->setRoadSystemPtr( m_roadSystem );
		}*/
	}

	void Application::updateVehicles( double deltaTime )
	{
		for( auto currentVehicle = m_vehicles.begin(); currentVehicle != m_vehicles.end(); ++currentVehicle )
		{
			(*currentVehicle)->updateSimulation( deltaTime );
		}

		//check the number of vehicles
		for( int i = m_vehicles.size(); i < MAX_CAR_NUMBER; ++i )
		{
			m_vehicles.push_back( new Vehicle( 100.f ) );
			m_vehicles.back()->setRoadSystemPtr( m_roadSystem );
		}
	}

	void Application::renderVehicles()
	{
		for( auto currentVehicle = m_vehicles.begin(); currentVehicle != m_vehicles.end(); ++currentVehicle )
		{
			m_matrixStack.PushMatrix();
			(*currentVehicle)->render( m_matrixStack );
			m_matrixStack.PopMatrix();
		}
	}

	Application::~Application()
	{
		//delete the road system
		delete m_roadSystem;

		delete m_vehicleManager;

		if(m_viewport)
		delete m_viewport;
	}

	bool Application::AddRoad(RoadNodeCluster* roadToAdd)
	{
		if(!roadToAdd->m_roadNodes.empty())
		{
			roadToAdd->InitiateNodeConnections();
			m_roadNodeClusters.push_back(roadToAdd);

			return true;
		}

		return false;
	}

	RoadNodeIntersection* Application::ConvertNodeToIntersection(RoadNodeCluster* intersectionRoadNodeCluster, int intersectionNodeIndex)
	{
		//find the road node cluster
		int roadNodeClusterIndex = -1;
		for(int nodeIndex = 0; nodeIndex < m_roadNodeClusters.size(); ++nodeIndex)
		{
			if(m_roadNodeClusters[nodeIndex] == intersectionRoadNodeCluster)
			{
				roadNodeClusterIndex = nodeIndex;
				break;
			}
		}

		//now that we have the index of the road node cluster, lets split the road node
		//there are two edgecases
		//connecting to the first node
		//connecting to the last node
		//we will take care of those edgecases later

		RoadNodeCluster* roadBeforeIntersection = new RoadNodeCluster();
		RoadNodeCluster* roadAfterIntersection = new RoadNodeCluster();

		for(int nodeIndex = 0; nodeIndex < intersectionNodeIndex; ++nodeIndex)
		{
			roadBeforeIntersection->AddNode(intersectionRoadNodeCluster->m_roadNodes[nodeIndex]);
		}

		for(int nodeIndex = intersectionNodeIndex + 1; nodeIndex < intersectionRoadNodeCluster->m_roadNodes.size(); ++nodeIndex)
		{
			roadAfterIntersection->AddNode(intersectionRoadNodeCluster->m_roadNodes[nodeIndex]);
		}

		if(roadNodeClusterIndex > -1)
		{
			m_roadNodeClusters.erase(m_roadNodeClusters.begin() + roadNodeClusterIndex);
		}
		RoadNodeIntersection* newIntersection = new RoadNodeIntersection(intersectionRoadNodeCluster->m_roadNodes[intersectionNodeIndex]);

		//check the size of each new road
		AddRoad(roadAfterIntersection);
		AddRoad(roadBeforeIntersection);

		m_intersectionNodes.push_back(newIntersection);
		return newIntersection;
	}

	void Application::readMouseClick(const Vector2& mouseClickLocation)
	{
		Vector3 mouseWorldPosition;
		if(m_splineMode)
		{
			if( (m_indexOfLastTempNode - m_indexOfLastPermanentNode) > 0 )
			{
				//add all the nodes to the current road
				for(int i = m_indexOfLastPermanentNode + 1; i <= m_indexOfLastTempNode; ++i)
				{
					//add the nodes to the current node holder
					m_currentRoadNodeCluster->AddNode(m_tempNodes[i]);
					m_tempNodes[i]->m_isValid = true;
				}
				m_currentRoadNodeCluster->InitiateNodeConnections();
				m_indexOfLastPermanentNode = m_indexOfLastTempNode;

				//check if we are forking the road
				if(m_forkIntersectionNode)
				{
					RoadNodeIntersection* newIntersection = ConvertNodeToIntersection(m_roadNodeClusterToSpawnFork, m_indexOfForkNode);
					newIntersection->AddOutgoingRoadNode(m_currentRoadNodeCluster->m_roadNodes[0]);

					m_forkIntersectionNode = nullptr;
					m_roadNodeClusterToSpawnFork = nullptr;
					m_indexOfForkNode = -1;
				}

				//check if we are interacting with a permanent node
				if(m_roadNodeInRange)
				{
					//Are we creating a new intersection
					if(m_roadNodeClusterInRange)
					{
						//check if we are inserting into the start of the road
						if(m_intersectionNodeIndex == 0)
						{
							if(m_roadNodeClusterInRange != m_currentRoadNodeCluster)
							{
								//find the index of the roadNodeCluster we are attempting to connect to
								int intersectionRoadNodeClusterIndex = 0;
								for(; intersectionRoadNodeClusterIndex < m_roadNodeClusters.size(); ++intersectionRoadNodeClusterIndex)
								{
									if(m_roadNodeClusters[intersectionRoadNodeClusterIndex] == m_roadNodeClusterInRange)
									{
										break;
									}
								}

								//erase the roadNodeCluster from the list of roadNodeClusters
								m_roadNodeClusters.erase(m_roadNodeClusters.begin() + intersectionRoadNodeClusterIndex);

								//add the nodes of the removed road to the current road
								for(int roadNodeIndex = 0; roadNodeIndex < m_roadNodeClusterInRange->m_roadNodes.size(); ++roadNodeIndex)
								{
									m_currentRoadNodeCluster->AddNode(m_roadNodeClusterInRange->m_roadNodes[roadNodeIndex]);
								}
								delete m_roadNodeClusterInRange;
							}
							else
							{
								m_currentRoadNodeCluster->m_roadNodes[0]->AddNextNode(m_currentRoadNodeCluster->m_roadNodes.back());
								m_currentRoadNodeCluster->m_roadNodes.back()->AddPreviousNode(m_currentRoadNodeCluster->m_roadNodes[0]);
							}

							m_currentRoadNodeCluster->InitiateNodeConnections();
							m_roadNodeClusterInRange = nullptr;
						}
						else
						{
							RoadNodeIntersection* newIntersection = ConvertNodeToIntersection(m_roadNodeClusterInRange,
								m_intersectionNodeIndex);
							newIntersection->AddIncomingRoadNode(m_tempNodes[m_indexOfLastPermanentNode]);
						}

						ExitSplineMode();
					}
					//at this point we are interacting with an existing intersection
					else
					{
						ExitSplineMode();
					}
				}
			}
		}
		else
		{
			//Free-form mode
			if( GetMouseWorldPosWithSpecifiedZ(mouseWorldPosition, 0.f) )
			{
				RoadNode* currentRoadNode = nullptr;
				RoadNodeIntersection* closestIntersectionNode = nullptr;
				RoadNodeCluster* currentRoadNodeCluster = nullptr;

				int indexOfClosestNode = -1;
				CheckForRoadNodesWithinRange(mouseWorldPosition, currentRoadNodeCluster, currentRoadNode, indexOfClosestNode);
				if(currentRoadNodeCluster == nullptr)
				{
					CheckForIntersectionNodesWithinRange(mouseWorldPosition, closestIntersectionNode, indexOfClosestNode);
					currentRoadNode = closestIntersectionNode;
				}

				if(currentRoadNode != nullptr)
				{
					//check if this is forking from a road node or an intersection node
					if(currentRoadNodeCluster)
					{
						for(int i = 0; i < currentRoadNodeCluster->m_roadNodes.size(); ++i)
						{
							m_tempNodes.push_back(currentRoadNodeCluster->m_roadNodes[i]);

							if(currentRoadNodeCluster->m_roadNodes[i] == currentRoadNode)
							{
								m_indexOfLastPermanentNode = i;
								break;
							}
						}

						//check if this is going to become an intersection node
						if(currentRoadNode->m_nextNodes.empty())
						{
							//we know this is the end node, meaning that the currentRoadNode is not going to become an intersection
							m_currentRoadNodeCluster = currentRoadNodeCluster;
							m_currentRoadIsNew = false;
						}
						else
						{
							m_currentRoadNodeCluster = new RoadNodeCluster();
							m_currentRoadIsNew = true;
							m_forkIntersectionNode = currentRoadNode;
							m_roadNodeClusterToSpawnFork = currentRoadNodeCluster;
							m_indexOfForkNode = indexOfClosestNode;
						}

						m_splineMode = true;
					}
					else
					{
						//at this point we know we are branching of an intersection node
						if(closestIntersectionNode->AllowsForOutgoingNodes())
						{
							m_tempNodes.push_back(currentRoadNode);
							m_indexOfLastPermanentNode = 0;
							m_roadNodeIntersectionToForkFrom = closestIntersectionNode;
							m_currentRoadNodeCluster = new RoadNodeCluster();
							m_currentRoadIsNew = true;
							m_splineMode = true;
						}
					}
					return;
				}
				else
				{
					//we are not forking a road
					RoadNode* newRoadNode = new RoadNode(mouseWorldPosition);

					if(!m_currentRoadNodeCluster)
					{
						//create a new node cluster
						m_currentRoadNodeCluster = new RoadNodeCluster();
					}

					m_currentRoadIsNew = true;
					//add the node to the current node holder
					m_currentRoadNodeCluster->AddNode(newRoadNode);
					m_tempNodes.push_back(newRoadNode);
				}

				m_splineMode = true;
			}
		}
	}

	void Application::CheckForRoadNodesWithinRange(const Vector3& worldPosition, RoadNodeCluster*& out_roadNodeCluster,
													RoadNode*& out_roadNode, int& out_indexOfClosestNode)
	{
		//get the closest node
		float closestDistanceSquared = -1.f;
		float currentDistanceSquared = 0.f;

		//iterate through all the roadnodeClusters
		RoadNodeCluster* currentRoadNodeCluster = nullptr;
		RoadNode* currentRoadNode = nullptr;

		//first we check if we are hitting the current road node cluster
		if( m_currentRoadNodeCluster != nullptr )
		{
			for(int i = 0; i < m_currentRoadNodeCluster->m_roadNodes.size(); ++i)
			{
				currentRoadNode = m_currentRoadNodeCluster->m_roadNodes[i];
				currentDistanceSquared = (currentRoadNode->m_location - worldPosition).calculateRadialDistanceSquared();

				if(currentDistanceSquared < closestDistanceSquared
					|| closestDistanceSquared < 0.f )
				{
					closestDistanceSquared = currentDistanceSquared;
					out_roadNode = currentRoadNode;
					out_roadNodeCluster = m_currentRoadNodeCluster;
					out_indexOfClosestNode = i;
				}
			}
		}

		for(int i = 0; i < m_roadNodeClusters.size(); ++i)
		{
			currentRoadNodeCluster = m_roadNodeClusters[i];

			for(int j = 0; j < currentRoadNodeCluster->m_roadNodes.size(); ++j)
			{
				currentRoadNode = currentRoadNodeCluster->m_roadNodes[j];
				currentDistanceSquared = (currentRoadNode->m_location - worldPosition).calculateRadialDistanceSquared();

				if(currentDistanceSquared < closestDistanceSquared
					|| closestDistanceSquared < 0.f )
				{
					closestDistanceSquared = currentDistanceSquared;
					out_roadNode = currentRoadNode;
					out_roadNodeCluster = currentRoadNodeCluster;
					out_indexOfClosestNode = j;
				}
			}
		}

		if( sqrtf(closestDistanceSquared) > sqrtf(m_lengthOfFragmentSquared) * 0.5f )
		{
			out_roadNode = nullptr;
			out_roadNodeCluster = nullptr;
			out_indexOfClosestNode = -1;
		}
	}

	void Application::CheckForIntersectionNodesWithinRange(const Vector3& worldPosition, RoadNodeIntersection*& out_intersectionNode,
		int& out_indexOfIntersectionNode)
	{
		//get the closest node
		float closestDistanceSquared = -1.f;
		float currentDistanceSquared = 0.f;

		//iterate through all the intersection road nodes
		RoadNodeIntersection* intersectionNode = nullptr;

		for(int i = 0; i < m_intersectionNodes.size(); ++i)
		{
			intersectionNode = m_intersectionNodes[i];
			currentDistanceSquared = (intersectionNode->m_location - worldPosition).calculateRadialDistanceSquared();

			if(currentDistanceSquared < closestDistanceSquared
				|| closestDistanceSquared < 0.f )
			{
				closestDistanceSquared = currentDistanceSquared;
				out_intersectionNode = intersectionNode;
				out_indexOfIntersectionNode = i;
			}
		}

		if( closestDistanceSquared > m_lengthOfFragmentSquared )
		{
			out_intersectionNode = nullptr;
			out_indexOfIntersectionNode = -1;
		}
	}

	void Application::ExitSplineMode()
	{
		if(m_currentRoadIsNew)
		{
			if(m_currentRoadNodeCluster->m_roadNodes.size() > 1)
			{
				AddRoad(m_currentRoadNodeCluster);
			}
			else
			{
				delete m_currentRoadNodeCluster;
			}
		}

		for(int nodeIndex = m_indexOfLastPermanentNode + 1; nodeIndex < m_tempNodes.size(); ++nodeIndex)
		{
			if(m_tempNodes[nodeIndex])
			{
				delete m_tempNodes[nodeIndex];
			}
		}

		m_tempNodes.clear();
		m_indexOfLastPermanentNode = 0;
		m_indexOfLastTempNode = 0;
		m_splineMode = false;
		m_currentRoadNodeCluster = nullptr;
		m_currentRoadIsNew = false;
		m_forkIntersectionNode = nullptr;
		m_roadNodeIntersectionToForkFrom = nullptr;
		m_roadNodeClusterToSpawnFork = nullptr;
		m_indexOfForkNode = -1;
	}

	bool Application::GetMouseWorldPosWithSpecifiedZ( Vector3& out_worldPos, float desiredZValue )
	{
		float zSlope = m_mouseFarFrustumCoordinates.z - m_mouseNearFrustumCoordinates.z;
		if( m_mouseNearFrustumCoordinates.z >= desiredZValue && zSlope < 0.f
			|| m_mouseNearFrustumCoordinates.z < desiredZValue && zSlope > 0.f)
		{
			//we know that the plane z == 0.f is between the front and back part of the frustum
			//calculate how far to get to it
			float t = (desiredZValue-m_mouseNearFrustumCoordinates.z) / (m_mouseFarFrustumCoordinates.z - m_mouseNearFrustumCoordinates.z);
			out_worldPos = ((1.f - t) * m_mouseNearFrustumCoordinates) + (t * m_mouseFarFrustumCoordinates);

			return true;
		}

		return false;
	}
}