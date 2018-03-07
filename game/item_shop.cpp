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

CItemShop::CItemShop(LPCHARACTER owner) : m_bItemLoaded(false), m_bCategoryLoaded(false), m_pkOwner(owner)
{
	memset(m_pkItems, 0, sizeof(m_pkItems));
	memset(m_pkCategories, 0, sizeof(m_pkCategories));
}

CItemShop::~CItemShop()
{
	for (int i = 0; i < ITEMSHOP_CATEGORY_MAX_NUM; ++i)
	{
	    if (m_pkCategories[i])
	    {
            delete m_pkCategories[i];
            m_pkCategories[i] = NULL;
            for (int j = 0; j < ITEMSHOP_MAX_NUM; ++j)
            {
                if (m_pkItems[i][j])
                {
                    delete m_pkItems[i][j];
                    m_pkItems[i][j] = NULL;
                }
            }
        }
    }
}

void CItemShop::LoadCategory(TItemShopCategory* pCategory, DWORD dwSize)
{
    if (m_bCategoryLoaded)
        return;

    memset(m_pkCategories, 0, sizeof(m_pkCategories));

    for (int i=0; i < dwSize; i++)
    {
        TItemShopCategory category;
        category.category = pCategory[i].category;
        category.szCategoryName = pCategory[i].szCategoryName;
        m_pkCategories[pCategory[i].category] = category;
    }

    m_bCategoryLoaded = true;
}

void CItemShop::LoadItem(TItemShopItem* pItems, DWORD dwSize)
{
	if (m_bItemLoaded)
		return;

	memset(m_pkItems, 0, sizeof(m_pkItems));
	for (int i = 0; i < dwSize; ++i)
	{
		const TItemTable* pProto = ITEM_MANAGER::instance().GetTable(pItems[i].vnum);
		if (!pProto)
		{
			sys_err("cannot load itemshop item %u vnum %u for %u", pItems[i].id, pItems[i].vnum, m_pkOwner->GetName());
			continue;
		}

		m_pkItems[pItems[i].category][pItems[i].pos] = new TItemShopItem(pItems[i]);
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

	if (!m_pkCategoriesLoaded)
	{
		if (test_server)
			sys_log(0, "CItemShop::OpenItemShop: Request loading from DB");
		//db_clientdesc->DBPacket(HEADER_GD_GUILD_SAFEBOX_LOAD, ch->GetDesc()->GetHandle(), &dwGuildID, sizeof(DWORD));
		return;
	}

	if (!m_bItemLoaded)
	{
		if (test_server)
			sys_log(0, "CItemShop::OpenItemShop: Request loading from DB");
		//db_clientdesc->DBPacket(HEADER_GD_GUILD_SAFEBOX_LOAD, ch->GetDesc()->GetHandle(), &dwGuildID, sizeof(DWORD));
		return;
	}

	for (int i = 0; i < ITEMSHOP_CATEGORY_MAX_NUM; ++i)
	{
	    if (TItemShopCategory* pkCategory = __GetCategory(i))
	    {
	        TItemShopCategory category;
	        category.category = pkCategory->category;
	        category.szCategoryName = pkCategory->szCategoryName;
	        __ClientPacket(IS_SUB_HEADER_CATEGORY, &category, sizeof(SPacketGCItemShop), ch);
            for (int j = 0; j < ITEMSHOP_MAX_NUM; ++j)
            {
                if (TItemShopItem* pkItem = __GetItem(i, j))
                {
                    TItemShopItem item;
                    item.vnum = pkItem->vnum;
                    item.count = pkItem->count;
                    item.pos = pkItem->pos;
                    item.category = pkItem->category;
                    thecore_memcpy(item.sockets, pkItem->alSockets, sizeof(item.sockets));
                    thecore_memcpy(item.attr, pkItem->aAttr, sizeof(item.attr));

                    __ClientPacket(IS_SUB_HEADER_ITEM, &item, sizeof(SPacketGCItemShop), ch);
                }
            }
	    }
    }

	__ClientPacket(IS_SUB_HEADER_OPEN, &m_bSize, sizeof(BYTE), ch);

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

TItemShopItem* CItemShop::__GetItem(BYTE bCategory, WORD wPos)
{
	if (wPos >= ITEMSHOP_MAX_NUM)
		return NULL;
	if (bCategory >= ITEMSHOP_CATEGORY_MAX_NUM)
	    return NULL;
	return m_pkItems[bCategory][wPos];
}

TItemShopCategory* CItemShop::__GetCategory(BYTE bCategory)
{
	if (bCategory >= ITEMSHOP_CATEGORY_MAX_NUM)
	    return NULL;
	return m_pkCategories[bCategory];
}
#endif