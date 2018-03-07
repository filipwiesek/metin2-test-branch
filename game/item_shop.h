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

        void LoadItem(TPlayerItem* pItems, DWORD dwSize);
        void OpenItemShop(LPCHARACTER ch);

    private:
        void __ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch);

        bool m_bItemLoaded,
        bool m_bCategoryLoaded,
        TPlayerItem*			m_pkItems[ITEMSHOP_MAX_NUM];
	    TItemShopCategory*		m_bCategories[ITEMSHOP_CATEGORY_MAX_NUM];
	    LPCHARACTER owner


};
#endif
