#ifndef INCLUDE_ITEM_SHOP_H
#define INCLUDE_ITEM_SHOP_H
#include "stdafx.h"

#ifdef __ITEM_SHOP__

class CHARACTER;

class CItemShop
{
    public:
         CItemShop(LPCHARACTER owner);
        ~CItemShop();

        void LoadCategory(TItemShopCategory* pCategory, DWORD dwSize);
        void LoadItem(TPlayerItem* pItems, DWORD dwSize);
        void OpenItemShop(LPCHARACTER ch);

    private:
        void __ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch);
        TPlayerItem* __GetItem(BYTE bCategory, WORD wPos);
        TItemShopCategory* __GetCategory(BYTE bCategory);

        bool m_bItemLoaded,
        bool m_bCategoryLoaded,
        TItemShopItem*			m_pkItems[ITEMSHOP_CATEGORY_MAX_NUM][ITEMSHOP_MAX_NUM];
	    TItemShopCategory*		m_pkCategories[ITEMSHOP_CATEGORY_MAX_NUM];
	    LPCHARACTER owner


};
#endif
