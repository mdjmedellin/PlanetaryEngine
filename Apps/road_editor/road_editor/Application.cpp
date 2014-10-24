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
	Application::Application( HWND handleWnd, HDC handleDC )
		:	m_hWnd( handleWnd )
		,	m_hDC( handleDC )
		,	m_openCommandConsole( false )
		,	m_displayOrigin( false )
		,	m_matrixStack( MatrixStack() )
		,	m_windowSize( Vector2() )
		,	m_backgroundColor( Rgba( 0.0f, 0.0f, 0.0f ) )
		,	m_roadSystem( nullptr )
		,	m_camera( nullptr )
		,	m_vehicleManager( nullptr )
		,	m_updateInput( false )
		,	m_stop( false )
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
		initiateDrivingSystem();

		m_camera = new Camera( false );
		m_camera->setPosition( Vector3( 300.f, 300.f, 1500.f ) );

		m_updateInput = true;
	}

	void Application::onResize( int width, int height )
	{
		//keep track of the new window size
		m_windowSize.x = (float)width;
		m_windowSize.y = (float)height;
		theRenderer.updateWindowSize( m_windowSize );
		theConsole.recalculateText();

		glViewport( 0, 0, width, height );

		GLdouble aspectRatio = width / static_cast< GLdouble >( height );

		glMatrixMode( GL_MODELVIEW );
		m_matrixStack.ClearStack();
		m_matrixStack.LoadIdentity();
		m_matrixStack.PushPerspective( 45.f, (float)aspectRatio, .1f, 10000.f );
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


	void Application::updateFrame( double deltaTime )
	{	
		if( !m_openCommandConsole && m_updateInput )
		{
			m_camera->updateInput();
			m_camera->updateDynamics( deltaTime );

			//update the roads and vehicles
			m_roadSystem->update( deltaTime );
			m_vehicleManager->updateVehicles( deltaTime );
			//updateVehicles( deltaTime );
		}

		m_matrixStack.PushMatrix();
		theRenderer.useRegularFBO( m_backgroundColor.getVector4(), CLEAR_COLOR | CLEAR_DEPTH );

		render3DScene();

		m_matrixStack.PopMatrix();
		SwapBuffers( m_hDC );
	}

	void Application::toggleOrigin()
	{
		m_displayOrigin = !m_displayOrigin;
	}

	void Application::toggleCommandConsole()
	{
		m_openCommandConsole = !m_openCommandConsole;
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

		m_roadSystem->render( m_matrixStack );
		m_vehicleManager->renderVehicles( m_matrixStack );
		//renderVehicles();

		glLoadMatrixf( m_matrixStack.GetTopMatrixFV() );

		//glLineWidth( 4.f );
		//drawOrigin( 100 );
		//glLineWidth( 1.f );
		glEnable( GL_TEXTURE_2D );

		m_matrixStack.PopMatrix();
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
				PostQuitMessage( 0 );
				m_stop = true;
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
	}

}