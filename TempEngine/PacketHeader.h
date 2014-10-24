#ifndef PACKET_HEADER_H
#define PACKET_HEADER_H
#pragma once


enum PacketCommandType{	CMD_NONE,
						CMD_LOGIN,
						CMD_LOGOUT,
						CMD_KEEPALIVE,
						CMD_RTT,
						CMD_RTT_ACK,
						CMD_UPDATE_INPUT,
						CMD_ACK,
						CMD_UPDATE_ACTOR,
						CMD_REMOVE_ACTOR,
						CMD_ADD_BULLET,
						CMD_ADD_CLIENT,
						CMD_ADD_SHIP,
						CMD_RESPAWN,
						CMD_SYNCHRONIZE_CLOCK,
						CMD_NUMBER_OF_MESSAGE_TYPE };

namespace gh
{
	class PacketHeader
	{
	public:
		PacketHeader( const PacketCommandType& packetType = CMD_NONE );

		void reset( const PacketCommandType& packetType = CMD_NONE );
		PacketCommandType getType();
		unsigned long long getPacketNumber();

	private:
		PacketCommandType m_type;
		unsigned long long m_packetNumber;
	};
}

#endif