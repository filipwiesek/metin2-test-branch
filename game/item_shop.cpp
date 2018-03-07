#include "stdafx.h"

#ifdef __ITEM_SHOP__
#include "../common/length.h"
#include "item_shop.h"
#include "db.h"
#include "desc_client.h"
#include "item.h"
#include "char.h"
#include "item_manager.h"
#include "desc.h"
#include "config.h"
#include "buffer_manager.h"

CItemShop::CItemShop(LPCHARACTER owner) : m_bItemLoaded(false), m_pkOwner(owner)
{
	memset(m_pkItems, 0, sizeof(m_pkItems));
	memset(m_bItemGrid, 0, sizeof(m_bItemGrid));
}

CItemShop::~CItemShop()
{
	for (int i = 0; i < ITEMSHOP_MAX_NUM; ++i)
	{
		if (m_pkItems[i])
		{
			delete m_pkItems[i];
			m_pkItems[i] = NULL;
		}
	}
}

void CItemShop::LoadItem(TPlayerItem* pItems, DWORD dwSize)
{
	if (m_bItemLoaded)
		return;

	memset(m_pkItems, 0, sizeof(m_pkItems));
	memset(m_bItemGrid, 0, sizeof(m_bItemGrid));
	for (int i = 0; i < dwSize; ++i)
	{
		const TItemTable* pProto = ITEM_MANAGER::instance().GetTable(pItems[i].vnum);
		if (!pProto)
		{
			sys_err("cannot load itemshop item %u vnum %u for %u", pItems[i].id, pItems[i].vnum, m_pkOwner->GetName());
			continue;
		}

		m_pkItems[pItems[i].pos] = new TPlayerItem(pItems[i]);
		for (int iSize = 0; iSize < pProto->bSize; ++iSize)
			m_bItemGrid[pItems[i].pos + GUILD_SAFEBOX_ITEM_WIDTH * iSize] = 1;
	}

	m_bItemLoaded = true;
}

void CItemShop::OpenItemShop(LPCHARACTER ch)
{

	if (m_pkOwner == NULL || ch == NULL)
	{
		sys_err("cannot open itemshop for player [%d] %s", ch->GetPlayerID(), ch->GetName());
		return;
	}

	if (!m_bItemLoaded)
	{
		if (test_server)
			sys_log(0, "CItemShop::OpenItemShop: Request loading from DB");
		//db_clientdesc->DBPacket(HEADER_GD_GUILD_SAFEBOX_LOAD, ch->GetDesc()->GetHandle(), &dwGuildID, sizeof(DWORD));
		return;
	}

	__ClientPacket(IS_SUB_HEADER_OPEN, &m_bSize, sizeof(BYTE), ch);

	for (int i = 0; i < ITEMSHOP_MAX_NUM; ++i)
	{
		if (TPlayerItem* pkItem = __GetItem(i))
		{
			TItemShopItem item;
			item.vnum = pkItem->vnum;
			item.count = pkItem->count;
			item.pos = pkItem->pos;
			item.category = pkItem->category;
			thecore_memcpy(item.sockets, pkItem->alSockets, sizeof(item.sockets));
			thecore_memcpy(item.attr, pkItem->aAttr, sizeof(item.attr));

			__ClientPacket(GUILD_SAFEBOX_SUBHEADER_SET_ITEM, &item, sizeof(SPacketGCItemShop), ch);
		}
	}
}
void CItemShop::__ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch)
{
	SPacketGCItemShop packet;
	packet.header = HEADER_GC_ITEMSHOP;
	packet.size = sizeof(SPacketGCItemShop) + size;
	packet.subheader = subheader;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(SPacketGCItemShop));
	if (size)
		buf.write(c_pData, size);

	if (ch)
		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	else
		__ViewerPacket(buf.read_peek(), buf.size());
}
#endif