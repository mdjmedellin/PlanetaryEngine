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
}

namespace gh
{
	void RoadNode::Render(MatrixStack& matrixStack, const Vector3& nodeColor)
	{
		matrixStack.PushMatrix();
		matrixStack.Translate(m_location);

		glLoadMatrixf( matrixStack.GetTopMatrixFV() );

		Rgba myColor = Rgba::WHITE;
		myColor.m_R = nodeColor.x;
		myColor.m_G = nodeColor.y;
		myColor.m_B = nodeColor.z;

		AABB2 test(-1, -1, 1, 1);
		theRenderer.renderQuad(test, myColor.getVector4(), nullptr);
		matrixStack.PopMatrix();
	}

	void RoadNode::SetLocation(const Vector3& newLocation)
	{
		m_location = newLocation;
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
			Vector3 prevNodeLocation;
			for(int i = 1; i < m_roadNodes.size(); ++i)
			{
				currentNodeLocation = m_roadNodes[i-1]->m_location;
				prevNodeLocation = currentNodeLocation;
				glVertex3f(currentNodeLocation.x, currentNodeLocation.y, currentNodeLocation.z);
				currentNodeLocation = m_roadNodes[i]->m_location;
				glVertex3f(currentNodeLocation.x, currentNodeLocation.y, currentNodeLocation.z);

				++nodeCount;
				if(nodeCount >= nodesBetweenArrowHeads && renderDirection)
				{
					currentArrowHeadInformation.location = currentNodeLocation;
					currentArrowHeadInformation.direction = currentNodeLocation - prevNodeLocation;
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
		,	m_indexOfLastTempNode(0)
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable(GL_BLEND);

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

		//road system variables
		Vector3 unitVector(1.f, 0.f, 0.f);
		Vector3 rotatedVector = unitVector;
		rotatedVector.setPitchAndYawDegrees(m_maxYawRotationDegreesForRoadSegments, 0.f);

		m_maxTurnAngleDotProductForRoadSegments = rotatedVector.DotProduct(unitVector);
		m_roadMaxCWRotationTransformationMatrix = Matrix4X4::RotateZDegreesMatrix(-m_maxYawRotationDegreesForRoadSegments);
		m_roadMaxCCWRotationTransfromationMatrix = Matrix4X4::RotateZDegreesMatrix(m_maxYawRotationDegreesForRoadSegments);

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
			Vector2 mouseCoords2D(m_mouseScreenCoordinates.x, m_mouseScreenCoordinates.y);
			glLoadMatrixf( m_matrixStack.GetTopMatrixFV() );
			theRenderer.renderQuad( AABB2( mouseCoords2D - Vector2( 10, 10 ), mouseCoords2D + Vector2(10, 10 ) ),
				Vector4( 1.0, 1.0, 1.0, 1.0 ), NULL );

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

		glEnable( GL_TEXTURE_2D );
		m_matrixStack.PopMatrix();
	}


	bool Application::GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(const Vector3& desiredDirection,
		const Matrix4X4& maxRotationMatrix, float maxTurnAngleDotProduct, int indexOfLastValidNode, NodeInformation& out_info)
	{
		if(indexOfLastValidNode <= 1)
		{
			return false;
		}

		//Get the needed directions normalized
		Vector3 currentNormalizedDirection = m_tempNodes[indexOfLastValidNode]->m_location
									- m_tempNodes[indexOfLastValidNode - 1]->m_location;
		currentNormalizedDirection.normalize();
		Vector3 desiredNormalizedDirection = desiredDirection;
		desiredNormalizedDirection.normalize();

		float dotProductDesiredDirectionComparedToCurrentDirection = desiredNormalizedDirection.DotProduct(currentNormalizedDirection);

		out_info.location = m_tempNodes[indexOfLastValidNode]->m_location;

		//rotate the next fragment to be placed
		//check if we can turn the road segment to face the cursor directly
		if( maxTurnAngleDotProduct < dotProductDesiredDirectionComparedToCurrentDirection)
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

	int Application::AddSemiCircle(int indexOfSemiCircleStart, const Vector3& directionToBuildSemiCircle)
	{
		//we need at least two nodes in order to figure out our current direction
		if(indexOfSemiCircleStart <= 1
			|| m_tempNodes.size() <= 1)
		{
			return 0;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////
		//The following calculates the direction we should rotate the fragments when building the semicircle

		//rotation matrices we need for transformations
		Matrix4X4 clockwise90DegreeRotation = Matrix4X4::RotateZDegreesMatrix(-90.f);

		//calculate the normalized direction of the last segment
		Vector3 normalizedDirectionOfLastRoadSegment = m_tempNodes[indexOfSemiCircleStart]->m_location 
			- m_tempNodes[indexOfSemiCircleStart - 1]->m_location;
		normalizedDirectionOfLastRoadSegment.normalize();

		Vector3 normalizedDirectionOfLastRoadSegmentRotates90CW = 
			clockwise90DegreeRotation.TransformDirection(normalizedDirectionOfLastRoadSegment);

		//calculate the angle we want to be facing
		Vector3 desiredDirection = clockwise90DegreeRotation.TransformDirection(normalizedDirectionOfLastRoadSegment);
		desiredDirection = clockwise90DegreeRotation.TransformDirection(desiredDirection);

		//calculate the direction in which we are to rotate
		Matrix4X4 rotationMatrix = m_roadMaxCCWRotationTransfromationMatrix;
		if( normalizedDirectionOfLastRoadSegmentRotates90CW.DotProduct(directionToBuildSemiCircle) > 0)
		{
			rotationMatrix = m_roadMaxCWRotationTransformationMatrix;
		}
		//////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		//Adds nodes until the semicircle is built

		float currentDotProduct = -1.f;
		float previousDotProduct = -1.f;
		bool continueTurning = true;

		NodeInformation nextNodeInformation;

		int indexOfNewNode = indexOfSemiCircleStart;
		int sizeOfNodeHolder = m_tempNodes.size();
		while( continueTurning )
		{
			++indexOfNewNode;

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
					--indexOfNewNode;
					continueTurning = false;
					continue;
				}

				//check if we need to add a new RoadNode
				if(indexOfNewNode >= sizeOfNodeHolder)
				{
					m_tempNodes.push_back(new RoadNode());
				}

				m_tempNodes[indexOfNewNode]->SetLocation(nextNodeInformation.location);
			}
		}
		/////////////////////////////////////////

		return indexOfNewNode - indexOfSemiCircleStart;
	}

	void Application::CalculateSplineToMousePos()
	{
		if(!m_splineMode 
			|| m_indexOfLastPermanentNode < 0)
		{
			return;
		}

		Vector3 mouseWorldPos;
		if(GetMouseWorldPosWithSpecifiedZ(mouseWorldPos, 0.f))
		{
			RoadNode* currentNode = m_tempNodes[m_indexOfLastPermanentNode];

			//calculate the distance and direction from the last node to the mouse
			Vector3 deltaDistanceCurrentNodeToMouse = mouseWorldPos - currentNode->m_location;
			deltaDistanceCurrentNodeToMouse.z = 0.f;			//make sure to zero out the z axis

			Vector3 normalizedDirectionCurrentNodeToMouse = deltaDistanceCurrentNodeToMouse;
			normalizedDirectionCurrentNodeToMouse.normalize();

			float distanceSquared = deltaDistanceCurrentNodeToMouse.calculateRadialDistanceSquared();
			float distanceSquaredFromLastPermanentNode = distanceSquared;

			//variables used to recalculate the best path to the mouse cursor
			int indexOfTempNodeToPlace = m_indexOfLastPermanentNode;
			int sizeOfNodeHolder = m_tempNodes.size();
			Vector3 locationOfNextNode;

			if(m_indexOfLastPermanentNode == 0)
			{
				while(distanceSquared > m_lengthOfFragmentSquared)
				{
					++indexOfTempNodeToPlace;
					locationOfNextNode = currentNode->m_location + (normalizedDirectionCurrentNodeToMouse * m_lengthOfFragment);

					if(indexOfTempNodeToPlace >= sizeOfNodeHolder)
					{
						m_tempNodes.push_back(new RoadNode());
					}

					//instead of allocating a new node, just reuse the node that is already on the list
					m_tempNodes[indexOfTempNodeToPlace]->SetLocation(locationOfNextNode);

					//recalculate distance to mouse
					deltaDistanceCurrentNodeToMouse = mouseWorldPos - locationOfNextNode;
					distanceSquared = deltaDistanceCurrentNodeToMouse.calculateRadialDistanceSquared();

					currentNode = m_tempNodes[indexOfTempNodeToPlace];
				}

				m_indexOfLastTempNode = indexOfTempNodeToPlace;
			}
			else
			{
				Matrix4X4 clockwiseMaxRotation = Matrix4X4::RotateZDegreesMatrix(-m_maxYawRotationDegreesForRoadSegments);
				Matrix4X4 counterClockwiseMaxRotation = Matrix4X4::RotateZDegreesMatrix(m_maxYawRotationDegreesForRoadSegments);
				Matrix4X4 clockwise90DegreeRotation = Matrix4X4::RotateZDegreesMatrix(-90.f);

				//calculate the direction of the last segment
				Vector3 normalizedDirectionOfLastSegment = m_tempNodes[m_indexOfLastPermanentNode]->m_location
					- m_tempNodes[m_indexOfLastPermanentNode - 1]->m_location;
				normalizedDirectionOfLastSegment.normalize();

				Vector3 normalizedDirectionOfLastSegmentRotated90DegreesCW = 
					clockwise90DegreeRotation.TransformDirection(normalizedDirectionOfLastSegment);

				//Get the midpoints of the road circles that can be constructed from this node
				Vector3 midpoint1 = currentNode->m_location + (normalizedDirectionOfLastSegmentRotated90DegreesCW * m_radiusOfRoadCircle);
				Vector3 midpoint2 = currentNode->m_location - (normalizedDirectionOfLastSegmentRotated90DegreesCW * m_radiusOfRoadCircle);

				//check if the mouse is inside any of the two circles that can be constructed from the last road segment
				float distanceSquaredFromMidpoint1 = (mouseWorldPos - midpoint1).calculateRadialDistanceSquared();
				float distanceSquaredFromMidpoint2 = (mouseWorldPos - midpoint2).calculateRadialDistanceSquared();
				float roadCircleRadiusSquared = m_radiusOfRoadCircle * m_radiusOfRoadCircle;

				if(distanceSquaredFromMidpoint1 >= roadCircleRadiusSquared
					&& distanceSquaredFromMidpoint2 >= roadCircleRadiusSquared)
				{
					int firstNodeToNotRequireMaxBend = -1;

					//the mouse cursor can be directly reached
					//keep adding nodes if the distance is greater than the size of one segment
					NodeInformation nextNodeInformation;
					bool cannotBreak = true;
					while(distanceSquared > m_lengthOfFragmentSquared)
					{
						deltaDistanceCurrentNodeToMouse = mouseWorldPos - currentNode->m_location;
						normalizedDirectionCurrentNodeToMouse = deltaDistanceCurrentNodeToMouse;
						normalizedDirectionCurrentNodeToMouse.normalize();
						distanceSquared = deltaDistanceCurrentNodeToMouse.calculateRadialDistanceSquared();

						//calculate the direction of the last segment
						normalizedDirectionOfLastSegment = m_tempNodes[indexOfTempNodeToPlace]->m_location
							- m_tempNodes[indexOfTempNodeToPlace - 1]->m_location;
						normalizedDirectionOfLastSegment.normalize();

						normalizedDirectionOfLastSegmentRotated90DegreesCW = 
							clockwise90DegreeRotation.TransformDirection(normalizedDirectionOfLastSegment);

						//check if we are rotating clockwise or counterclockwise
						float dotProductLastSegment90CWAndDirectionCurrentNodeToMouse
							= normalizedDirectionOfLastSegmentRotated90DegreesCW.DotProduct(normalizedDirectionCurrentNodeToMouse);

						Matrix4X4 rotationMatrix = counterClockwiseMaxRotation;
						if(dotProductLastSegment90CWAndDirectionCurrentNodeToMouse > 0.f )
						{
							rotationMatrix = clockwiseMaxRotation;
						}

						++indexOfTempNodeToPlace;

						//get the location of the next fragment rotated towards the desired direction
						if(GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(normalizedDirectionCurrentNodeToMouse,
							rotationMatrix, m_maxTurnAngleDotProductForRoadSegments, indexOfTempNodeToPlace - 1, nextNodeInformation))
						{
							//check our stopping conditions
							float newDistanceSquared = (nextNodeInformation.location - mouseWorldPos).calculateRadialDistanceSquared();

							//Check if we can break from the loop now
							if(normalizedDirectionOfLastSegment.DotProduct(normalizedDirectionCurrentNodeToMouse)
								>= m_maxTurnAngleDotProductForRoadSegments
								&& cannotBreak)
							{
								cannotBreak = false;
								//save the index of the first node that could directly face the destination
								firstNodeToNotRequireMaxBend = indexOfTempNodeToPlace;
							}

							//we are getting farther away from the mouse
							//ignore the condition if the fragment is the first one we are attempting to place
							int numberOfFragmentsPlaced = indexOfTempNodeToPlace - m_indexOfLastPermanentNode;
							if(newDistanceSquared > distanceSquared
								&& !cannotBreak)
							{
								--indexOfTempNodeToPlace;
								break;
							}

							//check if we need to add a new RoadNode
							if(indexOfTempNodeToPlace >= sizeOfNodeHolder)
							{
								m_tempNodes.push_back(new RoadNode());
							}

							m_tempNodes[indexOfTempNodeToPlace]->SetLocation(nextNodeInformation.location);
							currentNode = m_tempNodes[indexOfTempNodeToPlace];
						}
						else
						{
							--indexOfTempNodeToPlace;
							break;
						}
					}

					if(firstNodeToNotRequireMaxBend > 0
						&& firstNodeToNotRequireMaxBend < m_tempNodes.size())
					{
						//calculate how much we want to rotate the segment
						int numberOfNodesNotRequiredToMaxBend = m_tempNodes.size() - firstNodeToNotRequireMaxBend;
						Vector3 directionOfFirstNodeNotRequiredToMaxBend = m_tempNodes[firstNodeToNotRequireMaxBend - 1]->m_location 
																		- m_tempNodes[firstNodeToNotRequireMaxBend - 2]->m_location;
						directionOfFirstNodeNotRequiredToMaxBend.normalize();

						Vector3 desiredDirection = mouseWorldPos - m_tempNodes[firstNodeToNotRequireMaxBend - 1]->m_location;
						desiredDirection.normalize();

						float dotProductResult = directionOfFirstNodeNotRequiredToMaxBend.DotProduct(desiredDirection);
						float angleBetweenVector = acosf(dotProductResult);

						angleBetweenVector /= numberOfNodesNotRequiredToMaxBend;
						Vector3 currentLocation = m_tempNodes[firstNodeToNotRequireMaxBend - 1]->m_location;

						//which way are we rotating
						//calculate the direction of the last segment
						Vector3 normalizedDirectionOfLastSegmentRotated90DegreesCW = 
							clockwise90DegreeRotation.TransformDirection(directionOfFirstNodeNotRequiredToMaxBend);

						Vector3 normalizedDirectionCurrentNodeRoadNodeToMouse = mouseWorldPos - currentLocation;
						normalizedDirectionCurrentNodeRoadNodeToMouse.normalize();

						//check if we are rotating clockwise or counterclockwise
						float dotProductLastSegment90CWAndDirectionCurrentNodeToMouse
							= normalizedDirectionOfLastSegmentRotated90DegreesCW.DotProduct(normalizedDirectionCurrentNodeRoadNodeToMouse);

						if(dotProductLastSegment90CWAndDirectionCurrentNodeToMouse > 0.f )
						{
							angleBetweenVector *= -1.f;
						}

						Matrix4X4 rotationVector = Matrix4X4::RotateZRadiansMatrix(angleBetweenVector);

						for(int i = firstNodeToNotRequireMaxBend; i < m_tempNodes.size(); ++i)
						{
							directionOfFirstNodeNotRequiredToMaxBend = rotationVector.TransformDirection(directionOfFirstNodeNotRequiredToMaxBend);
							currentLocation += directionOfFirstNodeNotRequiredToMaxBend * m_lengthOfFragment;
							m_tempNodes[i]->m_location = currentLocation;
						}
					}

				}
				else
				{
					//Check if we should add a semicircle
					if(distanceSquared > m_lengthOfFragment
						&& normalizedDirectionOfLastSegment.DotProduct(normalizedDirectionCurrentNodeToMouse) < 0
						&& true)
					{
						//the mouse is facing on the opposite direction of the last road fragment
						//therefore we need to add a semi circle turn
						indexOfTempNodeToPlace += AddSemiCircle(indexOfTempNodeToPlace, normalizedDirectionCurrentNodeToMouse);
						currentNode = m_tempNodes[indexOfTempNodeToPlace];
						deltaDistanceCurrentNodeToMouse = mouseWorldPos - currentNode->m_location;
						normalizedDirectionCurrentNodeToMouse = deltaDistanceCurrentNodeToMouse;
						normalizedDirectionCurrentNodeToMouse.normalize();
						distanceSquared = deltaDistanceCurrentNodeToMouse.calculateRadialDistanceSquared();
					}

					//keep adding nodes if the distance is greater than the size of one segment
					NodeInformation nextNodeInformation;
					while(distanceSquared > m_lengthOfFragmentSquared)
					{
						deltaDistanceCurrentNodeToMouse = mouseWorldPos - currentNode->m_location;
						normalizedDirectionCurrentNodeToMouse = deltaDistanceCurrentNodeToMouse;
						normalizedDirectionCurrentNodeToMouse.normalize();
						distanceSquared = deltaDistanceCurrentNodeToMouse.calculateRadialDistanceSquared();

						//calculate the direction of the last segment
						normalizedDirectionOfLastSegment = m_tempNodes[indexOfTempNodeToPlace]->m_location
							- m_tempNodes[indexOfTempNodeToPlace - 1]->m_location;
						normalizedDirectionOfLastSegment.normalize();

						normalizedDirectionOfLastSegmentRotated90DegreesCW = 
							clockwise90DegreeRotation.TransformDirection(normalizedDirectionOfLastSegment);

						//check if we are rotating clockwise or counterclockwise
						float dotProductLastSegment90CWAndDirectionCurrentNodeToMouse
							= normalizedDirectionOfLastSegmentRotated90DegreesCW.DotProduct(normalizedDirectionCurrentNodeToMouse);

						Matrix4X4 rotationMatrix = counterClockwiseMaxRotation;
						if(dotProductLastSegment90CWAndDirectionCurrentNodeToMouse > 0.f )
						{
							rotationMatrix = clockwiseMaxRotation;
						}

						++indexOfTempNodeToPlace;

						//get the location of the next fragment rotated towards the desired direction
						if(GetInformationOfNextRoadNodeRotatedTowardsTheDesiredDirection(normalizedDirectionCurrentNodeToMouse,
							rotationMatrix, m_maxTurnAngleDotProductForRoadSegments, indexOfTempNodeToPlace - 1, nextNodeInformation))
						{
							//check our stopping conditions
							float newDistanceSquared = (nextNodeInformation.location - mouseWorldPos).calculateRadialDistanceSquared();

							//we are getting farther away from the mouse
							//ignore the condition if the fragment is the first one we are attempting to place
							int numberOfFragmentsPlaced = indexOfTempNodeToPlace - m_indexOfLastPermanentNode;
							if(newDistanceSquared > distanceSquared
								&& numberOfFragmentsPlaced > 1)
							{
								--indexOfTempNodeToPlace;
								break;
							}

							//check if we need to add a new RoadNode
							if(indexOfTempNodeToPlace >= sizeOfNodeHolder)
							{
								m_tempNodes.push_back(new RoadNode());
							}

							m_tempNodes[indexOfTempNodeToPlace]->SetLocation(nextNodeInformation.location);
							currentNode = m_tempNodes[indexOfTempNodeToPlace];
						}
					}
				}

				m_indexOfLastTempNode = indexOfTempNodeToPlace;
			}
		}
	}

	void Application::RenderSplines()
	{
		//check if there are temporary splines
		if(m_splineMode && !m_tempNodes.empty())
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

		for(int i = 0; i < m_roadNodeClusters.size(); ++i)
		{
			m_roadNodeClusters[i]->Render(m_matrixStack, Vector3(1.f, 0.f, 0.f), m_showDirectionOnPlacedRoads);
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

		//print the strength of the start tangent
		std::string textToPrint;
		theRenderer.drawText( textToPrint.c_str(), "test", m_HUDFontHeight, textRenderWindow, TEXT_ALIGN_BOTTOM, toRGBA( 1.f, 1.f, 1.f ) );

		//print the strength of the end tangent
		float maxY = textRenderWindow.getMins().y;
		textRenderWindow.setMins(0.f, maxY - (m_HUDFontHeight + m_HUDLineBreakHeight));
		textRenderWindow.setMaxs(windowSize.x, maxY);
		theRenderer.drawText( textToPrint.c_str(), "test", m_HUDFontHeight, textRenderWindow, TEXT_ALIGN_BOTTOM, toRGBA( 1.f, 1.f, 1.f ) );
	}

	void Application::updateInput( bool continueUpdating )
	{
		m_updateInput = continueUpdating;
	}

	void Application::keyPressed( size_t charPressed )
	{
		if( (char)charPressed == '~' )
		{
			m_openCommandConsole = !m_openCommandConsole;
		}
		else if( m_openCommandConsole )
		{
			theConsole.keyPressed( charPressed );
		}
		else
		{
			switch( (char)charPressed )
			{
			case 27:
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

			default:
				break;
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

	void Application::readMouseClick(const Vector2& mouseClickLocation)
	{
		Vector3 mouseWorldPosition;
		if(m_splineMode)
		{
			if(m_indexOfLastPermanentNode != m_indexOfLastTempNode)
			{
				if(!m_currentRoadNodeCluster)
				{
					//create a new node holder
					m_currentRoadNodeCluster = new RoadNodeCluster();
				}

				//add all the nodes to the current road
				for(int i = m_indexOfLastPermanentNode + 1; i <= m_indexOfLastTempNode; ++i)
				{
					//add the nodes to the current node holder
					m_currentRoadNodeCluster->AddNode(m_tempNodes[i]);
				}

				m_indexOfLastPermanentNode = m_indexOfLastTempNode;
			}
		}
		else
		{
			//Free-form mode
			if(GetMouseWorldPosWithSpecifiedZ(mouseWorldPosition, 0.f))
			{
				RoadNode* newRoadNode = new RoadNode(mouseWorldPosition);

				if(!m_currentRoadNodeCluster)
				{
					//create a new node holder
					m_currentRoadNodeCluster = new RoadNodeCluster();
				}

				//add the nodes to the current node holder
				m_currentRoadNodeCluster->AddNode(newRoadNode);
				m_tempNodes.push_back(newRoadNode);
				m_splineMode = true;
			}
		}
	}

	void Application::ExitSplineMode()
	{
		if(m_splineMode 
			&& m_currentRoadNodeCluster
			&& m_currentRoadNodeCluster->m_roadNodes.size() > 1 )
		{
			m_roadNodeClusters.push_back(m_currentRoadNodeCluster);
			m_currentRoadNodeCluster = nullptr;
		}

		if(m_currentRoadNodeCluster)
		{
			delete m_currentRoadNodeCluster;
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