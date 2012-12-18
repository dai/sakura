/*!	@file
	@brief ���ʐݒ�_�C�A���O�{�b�N�X�A�u�L�[�o�C���h�v�y�[�W

	@author Norio Nakatani
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000-2001, genta, MIK, jepro
	Copyright (C) 2002, MIK, YAZAKI, aroka
	Copyright (C) 2003, KEITA
	Copyright (C) 2006, ryoji
	Copyright (C) 2007, ryoji
	Copyright (C) 2009, nasukoji

	This source code is designed for sakura editor.
	Please contact the copyright holders to use this code for other purpose.
*/


#include "StdAfx.h"
#include "CPropCommon.h"
#include "CDlgOpenFile.h"
#include "etc_uty.h"
#include "CSMacroMgr.h" // 2002/2/10 aroka
#include "mymessage.h"
#include "Debug.h" ///
#include <stdio.h>	/// 2002/2/3 aroka from here
#include "sakura.hh"

//	From Here Sept. 5, 2000 JEPRO ���p�J�^�J�i�̑S�p���ɔ�����������ύX(21��34)
#define STR_KEYDATA_HEAD_LEN  34
//	To Here Sept. 5, 2000
#define STR_KEYDATA_HEAD      "�e�L�X�g�G�f�B�^ �L�[�ݒ�t�@�C��\x1a"

const char STR_KEYDATA_HEAD2[] = "// �e�L�X�g�G�f�B�^�L�[�ݒ� Ver2";	//@@@ 2001.11.07 add MIK

#define STR_SHIFT_PLUS        _T("Shift+")  //@@@ 2001.11.08 add MIK
#define STR_CTRL_PLUS         _T("Ctrl+")  //@@@ 2001.11.08 add MIK
#define STR_ALT_PLUS          _T("Alt+")  //@@@ 2001.11.08 add MIK

//@@@ 2001.02.04 Start by MIK: Popup Help
static const DWORD p_helpids[] = {	//10700
	IDC_BUTTON_IMPORT,				HIDC_BUTTON_IMPORT_KEYBIND,		//�C���|�[�g
	IDC_BUTTON_EXPORT,				HIDC_BUTTON_EXPORT_KEYBIND,		//�G�N�X�|�[�g
	IDC_BUTTON_ASSIGN,				HIDC_BUTTON_ASSIGN,				//�L�[���蓖��
	IDC_BUTTON_RELEASE,				HIDC_BUTTON_RELEASE,			//�L�[����
	IDC_CHECK_SHIFT,				HIDC_CHECK_SHIFT,				//Shift�L�[
	IDC_CHECK_CTRL,					HIDC_CHECK_CTRL,				//Ctrl�L�[
	IDC_CHECK_ALT,					HIDC_CHECK_ALT,					//Alt�L�[
	IDC_COMBO_FUNCKIND,				HIDC_COMBO_FUNCKIND_KEYBIND,	//�@�\�̎��
	IDC_EDIT_KEYSFUNC,				HIDC_EDIT_KEYSFUNC,				//�L�[�Ɋ��蓖�Ă��Ă���@�\
	IDC_LIST_FUNC,					HIDC_LIST_FUNC_KEYBIND,			//�@�\�ꗗ
	IDC_LIST_KEY,					HIDC_LIST_KEY,					//�L�[�ꗗ
	IDC_LIST_ASSIGNEDKEYS,			HIDC_LIST_ASSIGNEDKEYS,			//�@�\�Ɋ��蓖�Ă��Ă���L�[
	IDC_LABEL_MENUFUNCKIND,			-1,
	IDC_LABEL_MENUFUNC,				-1,
	IDC_LABEL_KEYKIND,				-1,
	IDC_LABEL_FUNCtoKEY,			-1,
	IDC_LABEL_KEYtoFUNC,			-1,
	IDC_CHECK_ACCELTBL_EACHWIN,		HIDC_CHECK_ACCELTBL_EACHWIN,	// �E�B���h�E���ɃA�N�Z�����[�^�e�[�u�����쐬����(Wine�p)
//	IDC_STATIC,						-1,
	0, 0
};
//@@@ 2001.02.04 End

//	From Here Jun. 2, 2001 genta
/*!
	@param hwndDlg �_�C�A���O�{�b�N�X��Window Handle
	@param uMsg ���b�Z�[�W
	@param wParam �p�����[�^1
	@param lParam �p�����[�^2
*/
INT_PTR CALLBACK CPropCommon::DlgProc_PROP_KEYBIND(
	HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return DlgProc( &CPropCommon::DispatchEvent_p5, hwndDlg, uMsg, wParam, lParam );
}
//	To Here Jun. 2, 2001 genta

/* From Here Oct. 13, 2000 Studio C��Mr.�R�[�q�[���ɋ�����������ł������܂������Ă܂��� */
// �E�B���h�E�v���V�[�W���̒��ŁE�E�E
LRESULT CALLBACK CPropComKeybindWndProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg ) {
	// WM_CTLCOLORSTATIC ���b�Z�[�W�ɑ΂���
	case WM_CTLCOLORSTATIC:
	// ���F�̃u���V�n���h����Ԃ�
		return (LRESULT)GetStockObject(WHITE_BRUSH);
//	default:
//		break;
	}
	return 0;
}
/* To Here Oct. 13, 2000 */







/* Keybind ���b�Z�[�W���� */
INT_PTR CPropCommon::DispatchEvent_p5(
	HWND	hwndDlg,	// handle to dialog box
	UINT	uMsg,	// message
	WPARAM	wParam,	// first message parameter
	LPARAM	lParam 	// second message parameter
)
{
	WORD		wNotifyCode;
	WORD		wID;
	HWND		hwndCtl;
	NMHDR*		pNMHDR;
	NM_UPDOWN*	pMNUD;
	int			idCtrl;
	static HWND	hwndCombo;
	static HWND	hwndFuncList;
	static HWND	hwndKeyList;
	static HWND	hwndCheckShift;
	static HWND	hwndCheckCtrl;
	static HWND	hwndCheckAlt;
	static HWND	hwndAssignedkeyList;
//	static HWND hwndLIST_KEYSFUNC;
	static HWND hwndEDIT_KEYSFUNC;
//	int			nLength;
	int			nAssignedKeyNum;

	int			nIndex;
	int			nIndex2;
	int			nIndex3;
	int			i;
	int			j;
	int			nFuncCode;
	static TCHAR pszLabel[256];

	switch( uMsg ){
	case WM_INITDIALOG:
		/* �_�C�A���O�f�[�^�̐ݒ� Keybind */
		SetData_p5( hwndDlg );
		// Modified by KEITA for WIN64 2003.9.6
		::SetWindowLongPtr( hwndDlg, DWLP_USER, lParam );

		/* �R���g���[���̃n���h�����擾 */
		hwndCombo = ::GetDlgItem( hwndDlg, IDC_COMBO_FUNCKIND );
		hwndFuncList = ::GetDlgItem( hwndDlg, IDC_LIST_FUNC );
		hwndAssignedkeyList = ::GetDlgItem( hwndDlg, IDC_LIST_ASSIGNEDKEYS );
		hwndCheckShift = ::GetDlgItem( hwndDlg, IDC_CHECK_SHIFT );
		hwndCheckCtrl = ::GetDlgItem( hwndDlg, IDC_CHECK_CTRL );
		hwndCheckAlt = ::GetDlgItem( hwndDlg, IDC_CHECK_ALT );
		hwndKeyList = ::GetDlgItem( hwndDlg, IDC_LIST_KEY );
//		hwndLIST_KEYSFUNC = ::GetDlgItem( hwndDlg, IDC_LIST_KEYSFUNC );
		hwndEDIT_KEYSFUNC = ::GetDlgItem( hwndDlg, IDC_EDIT_KEYSFUNC );

		/* �L�[�I�����̏��� */
//	From Here Oct. 14, 2000 JEPRO �킩��ɂ����̂őI�����Ȃ��悤�ɕύX	//Oct. 17, 2000 JEPRO �����I
//	/* �L�[���X�g�̐擪�̍��ڂ�I���i���X�g�{�b�N�X�j*/
		::SendMessage( hwndKeyList, LB_SETCURSEL, (WPARAM)0, (LPARAM)0 );	//Oct. 14, 2000 JEPRO �������R�����g�A�E�g����Ɛ擪���ڂ��I������Ȃ��Ȃ�
		::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_LIST_KEY, LBN_SELCHANGE ), (LPARAM)hwndKeyList );	//Oct. 14, 2000 JEPRO �����͂ǂ����ł������H(�킩���)
//	To Here Oct. 14, 2000
		::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_COMBO_FUNCKIND, CBN_SELCHANGE ), (LPARAM)hwndCombo );

		::SetTimer( hwndDlg, 1, 300, NULL );	// 2007.11.02 ryoji

		return TRUE;

	case WM_NOTIFY:
		idCtrl = (int)wParam;
		pNMHDR = (NMHDR*)lParam;
		pMNUD  = (NM_UPDOWN*)lParam;
		switch( pNMHDR->code ){
		case PSN_HELP:
//			OnHelp( hwndDlg, IDD_PROP1P5 );		// Sept. 9, 2000 JEPRO ���ۂ�ID���ɕύX
			OnHelp( hwndDlg, IDD_PROP_KEYBIND );
			return TRUE;
		case PSN_KILLACTIVE:
//			MYTRACE_A( "Keybind PSN_KILLACTIVE\n" );
			/* �_�C�A���O�f�[�^�̎擾 Keybind */
			GetData_p5( hwndDlg );
			return TRUE;
//@@@ 2002.01.03 YAZAKI �Ō�ɕ\�����Ă����V�[�g�𐳂����o���Ă��Ȃ��o�O�C��
		case PSN_SETACTIVE:
			m_nPageNum = ID_PAGENUM_KEYBOARD;

			// �\�����X�V����i�}�N���ݒ��ʂł̃}�N�����ύX�𔽉f�j	// 2007.11.02 ryoji
			nIndex = ::SendMessage( hwndKeyList, LB_GETCURSEL, 0, 0 );
			nIndex2 = ::SendMessage( hwndCombo, CB_GETCURSEL, 0, 0 );
			nIndex3 = ::SendMessage( hwndFuncList, LB_GETCURSEL, 0, 0 );
			if( nIndex != LB_ERR ){
				::SendMessage( hwndDlg, WM_COMMAND, MAKEWPARAM( IDC_LIST_KEY, LBN_SELCHANGE ), (LPARAM)hwndKeyList );
			}
			if( nIndex2 != CB_ERR ){
				::SendMessage( hwndDlg, WM_COMMAND, MAKEWPARAM( IDC_COMBO_FUNCKIND, CBN_SELCHANGE ), (LPARAM)hwndCombo );
				if( nIndex3 != LB_ERR ){
					::SendMessage( hwndFuncList, LB_SETCURSEL, nIndex3, 0 );
				}
			}
			return TRUE;
		}
		break;

	case WM_COMMAND:
		wNotifyCode = HIWORD(wParam);	/* �ʒm�R�[�h */
		wID = LOWORD(wParam);	/* ����ID� �R���g���[��ID� �܂��̓A�N�Z�����[�^ID */
		hwndCtl = (HWND) lParam;	/* �R���g���[���̃n���h�� */

		switch( wNotifyCode ){
		/* �{�^���^�`�F�b�N�{�b�N�X���N���b�N���ꂽ */
		case BN_CLICKED:
			switch( wID ){
			case IDC_BUTTON_IMPORT:	/* �C���|�[�g */
				/* Keybind:�L�[���蓖�Đݒ���C���|�[�g���� */
				p5_Import_KeySetting( hwndDlg );
				return TRUE;
			case IDC_BUTTON_EXPORT:	/* �G�N�X�|�[�g */
				/* Keybind:�L�[���蓖�Đݒ���G�N�X�|�[�g���� */
				p5_Export_KeySetting( hwndDlg );
				return TRUE;
			case IDC_BUTTON_ASSIGN:	/* ���t */
				nIndex = ::SendMessage( hwndKeyList, LB_GETCURSEL, 0, 0 );
				nIndex2 = ::SendMessage( hwndCombo, CB_GETCURSEL, 0, 0 );
				nIndex3 = ::SendMessage( hwndFuncList, LB_GETCURSEL, 0, 0 );
				if( nIndex == LB_ERR || nIndex2 == CB_ERR || nIndex3 == LB_ERR ){
					return TRUE;
				}
				nFuncCode = m_cLookup.Pos2FuncCode( nIndex2, nIndex3 );	// Oct. 2, 2001 genta
				i = 0;
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_SHIFT ) ){
					i |= _SHIFT;
				}
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_CTRL ) ){
					i |= _CTRL;
				}
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_ALT ) ){
					i |= _ALT;
				}
				m_Common.m_pKeyNameArr[nIndex].m_nFuncCodeArr[i] = nFuncCode;
				::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_LIST_KEY, LBN_SELCHANGE ), (LPARAM)hwndKeyList );
				::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_LIST_FUNC, LBN_SELCHANGE ), (LPARAM)hwndFuncList );
				return TRUE;
			case IDC_BUTTON_RELEASE:	/* ���� */
				nIndex = ::SendMessage( hwndKeyList, LB_GETCURSEL, 0, 0 );
				if( nIndex == LB_ERR ){
					return TRUE;
				}
				nFuncCode = F_DEFAULT;
				i = 0;
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_SHIFT ) ){
					i |= _SHIFT;
				}
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_CTRL ) ){
					i |= _CTRL;
				}
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_ALT ) ){
					i |= _ALT;
				}
				m_Common.m_pKeyNameArr[nIndex].m_nFuncCodeArr[i] = nFuncCode;
				::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_LIST_KEY, LBN_SELCHANGE ), (LPARAM)hwndKeyList );
				::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_LIST_FUNC, LBN_SELCHANGE ), (LPARAM)hwndFuncList );
				return TRUE;
			}
			break;	/* BN_CLICKED */
		}
		if( hwndCheckShift == hwndCtl
		 || hwndCheckCtrl == hwndCtl
		 || hwndCheckAlt == hwndCtl
		){
			switch( wNotifyCode ){
			case BN_CLICKED:
				p5_ChangeKeyList( hwndDlg );

				return TRUE;
			}
		}else
		if( hwndKeyList == hwndCtl ){
			switch( wNotifyCode ){
			case LBN_SELCHANGE:
				nIndex = ::SendMessage( hwndKeyList, LB_GETCURSEL, 0, 0 );
				i = 0;
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_SHIFT ) ){
					i |= _SHIFT;
				}
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_CTRL ) ){
					i |= _CTRL;
				}
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_ALT ) ){
					i |= _ALT;
				}
				nFuncCode = m_Common.m_pKeyNameArr[nIndex].m_nFuncCodeArr[i];
				// Oct. 2, 2001 genta
				// 2007.11.02 ryoji F_DISABLE�Ȃ疢���t
				if( nFuncCode == F_DISABLE ){
					strcpy( pszLabel, _T("�����t") );
				}else{
					m_cLookup.Funccode2Name( nFuncCode, pszLabel, 255 );
				}
				::SetWindowText( hwndEDIT_KEYSFUNC, pszLabel );
				return TRUE;
			}
		}else
		if( hwndFuncList == hwndCtl ){
			switch( wNotifyCode ){
			case LBN_SELCHANGE:
				nIndex = ::SendMessage( hwndKeyList, LB_GETCURSEL, 0, 0 );
				nIndex2 = ::SendMessage( hwndCombo, CB_GETCURSEL, 0, 0 );
				nIndex3 = ::SendMessage( hwndFuncList, LB_GETCURSEL, 0, 0 );
				nFuncCode = m_cLookup.Pos2FuncCode( nIndex2, nIndex3 );	// Oct. 2, 2001 genta
				/* �@�\�ɑΉ�����L�[���̎擾(����) */
				CMemory**	ppcAssignedKeyList;
				nAssignedKeyNum = CKeyBind::GetKeyStrList(	/* �@�\�ɑΉ�����L�[���̎擾(����) */
					m_hInstance, m_Common.m_nKeyNameArrNum, (KEYDATA*)m_Common.m_pKeyNameArr,
					&ppcAssignedKeyList, nFuncCode,
					FALSE	// 2007.02.22 ryoji �f�t�H���g�@�\�͎擾���Ȃ�
				);	
				/* ���蓖�ăL�[���X�g���N���A���Ēl�̐ݒ� */
				::SendMessage( hwndAssignedkeyList, LB_RESETCONTENT, 0, 0 );
				if( 0 < nAssignedKeyNum){
					for( j = 0; j < nAssignedKeyNum; ++j ){
						/* �f�o�b�O���j�^�ɏo�� */
						const TCHAR* cpszString = ppcAssignedKeyList[j]->GetStringPtr();
						::SendMessage( hwndAssignedkeyList, LB_ADDSTRING, 0, (LPARAM)cpszString );
						delete ppcAssignedKeyList[j];
					}
					delete [] ppcAssignedKeyList;
				}
				return TRUE;
			}
		}else
		if( hwndCombo == hwndCtl){
			switch( wNotifyCode ){
			case CBN_SELCHANGE:
				nIndex2 = ::SendMessage( hwndCombo, CB_GETCURSEL, 0, 0 );
				/* �@�\�ꗗ�ɕ�������Z�b�g�i���X�g�{�b�N�X�j*/
				m_cLookup.SetListItem( hwndFuncList, nIndex2 );	//	Oct. 2, 2001 genta
				return TRUE;
			}

//@@@ 2001.11.08 add start MIK
		}else
		if( hwndAssignedkeyList == hwndCtl){
			switch( wNotifyCode ){
			case LBN_SELCHANGE:
			//case LBN_DBLCLK:
				{
					TCHAR	buff[1024], *p;
					int	ret;

					nIndex = ::SendMessage( hwndAssignedkeyList, LB_GETCURSEL, 0, 0 );
					memset(buff, 0, _countof(buff));
					ret = ::SendMessage( hwndAssignedkeyList, LB_GETTEXT, nIndex, (LPARAM)buff);
					if( ret != LB_ERR )
					{
						i = 0;
						p = buff;
						//SHIFT
						if( memcmp(p, STR_SHIFT_PLUS, _tcslen(STR_SHIFT_PLUS)) == 0 ){
							p += _tcslen(STR_SHIFT_PLUS);
							i |= _SHIFT;
						}
						//CTRL
						if( memcmp(p, STR_CTRL_PLUS, _tcslen(STR_CTRL_PLUS)) == 0 ){
							p += _tcslen(STR_CTRL_PLUS);
							i |= _CTRL;
						}
						//ALT
						if( memcmp(p, STR_ALT_PLUS, _tcslen(STR_ALT_PLUS)) == 0 ){
							p += _tcslen(STR_ALT_PLUS);
							i |= _ALT;
						}
						for(j = 0; j < m_Common.m_nKeyNameArrNum; j++)
						{
							if( _tcscmp(m_Common.m_pKeyNameArr[j].m_szKeyName, p) == 0 )
							{
								::SendMessage( hwndKeyList, LB_SETCURSEL, (WPARAM)j, (LPARAM)0);
								if( i & _SHIFT ) ::CheckDlgButton( hwndDlg, IDC_CHECK_SHIFT, BST_CHECKED );  //�`�F�b�N
								else             ::CheckDlgButton( hwndDlg, IDC_CHECK_SHIFT, BST_UNCHECKED );  //�`�F�b�N���͂���
								if( i & _CTRL )  ::CheckDlgButton( hwndDlg, IDC_CHECK_CTRL,  BST_CHECKED );  //�`�F�b�N
								else             ::CheckDlgButton( hwndDlg, IDC_CHECK_CTRL,  BST_UNCHECKED );  //�`�F�b�N���͂���
								if( i & _ALT )   ::CheckDlgButton( hwndDlg, IDC_CHECK_ALT,   BST_CHECKED );  //�`�F�b�N
								else             ::CheckDlgButton( hwndDlg, IDC_CHECK_ALT,   BST_UNCHECKED );  //�`�F�b�N���͂���
								::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_LIST_KEY, LBN_SELCHANGE ), (LPARAM)hwndKeyList );

								// �L�[�ꗗ�̕�������ύX
								p5_ChangeKeyList( hwndDlg );
								break;
							}
						}
					}
					return TRUE;
				}
			}
//@@@ 2001.11.08 add end MIK

		}
		break;

	case WM_TIMER:
		// �{�^���̗L���^������؂�ւ���	// 2007.11.02 ryoji
		nIndex = ::SendMessage( hwndKeyList, LB_GETCURSEL, 0, 0 );
		nIndex2 = ::SendMessage( hwndCombo, CB_GETCURSEL, 0, 0 );
		nIndex3 = ::SendMessage( hwndFuncList, LB_GETCURSEL, 0, 0 );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_ASSIGN ), !( LB_ERR == nIndex || nIndex2 == CB_ERR || nIndex3 == LB_ERR ) );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_RELEASE ), !( LB_ERR == nIndex ) );
		break;

	case WM_DESTROY:
		::KillTimer( hwndDlg, 1 );	// 2007.11.02 ryoji
		break;

//@@@ 2001.02.04 Start by MIK: Popup Help
	case WM_HELP:
		{
			HELPINFO *p = (HELPINFO *)lParam;
			MyWinHelp( (HWND)p->hItemHandle, m_pszHelpFile, HELP_WM_HELP, (ULONG_PTR)(LPVOID)p_helpids );	// 2006.10.10 ryoji MyWinHelp�ɕύX�ɕύX
		}
		return TRUE;
		/*NOTREACHED*/
		//break;
//@@@ 2001.02.04 End

//@@@ 2001.11.07 Start by MIK: Context Menu Help
	//Context Menu
	case WM_CONTEXTMENU:
		MyWinHelp( hwndDlg, m_pszHelpFile, HELP_CONTEXTMENU, (ULONG_PTR)(LPVOID)p_helpids );	// 2006.10.10 ryoji MyWinHelp�ɕύX�ɕύX
		return TRUE;
//@@@ 2001.11.07 End

	}
	return FALSE;
}







/* �_�C�A���O�f�[�^�̐ݒ� Keybind */
void CPropCommon::SetData_p5( HWND hwndDlg )
{
	HWND		hwndCombo;
	HWND		hwndKeyList;
	int			i;

	/* �@�\��ʈꗗ�ɕ�������Z�b�g�i�R���{�{�b�N�X�j*/
	hwndCombo = ::GetDlgItem( hwndDlg, IDC_COMBO_FUNCKIND );
	m_cLookup.SetCategory2Combo( hwndCombo );	//	Oct. 2, 2001 genta

	/* ��ʂ̐擪�̍��ڂ�I���i�R���{�{�b�N�X�j*/
	::SendMessage( hwndCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0 );	//Oct. 14, 2000 JEPRO JEPRO �u--����`--�v��\�������Ȃ��悤�ɑ匳 Funcode.cpp �ŕύX���Ă���

	/* �L�[�ꗗ�ɕ�������Z�b�g�i���X�g�{�b�N�X�j*/
	hwndKeyList = ::GetDlgItem( hwndDlg, IDC_LIST_KEY );
	for( i = 0; i < m_Common.m_nKeyNameArrNum; ++i ){
			::SendMessage( hwndKeyList, LB_ADDSTRING, 0, (LPARAM)m_Common.m_pKeyNameArr[i].m_szKeyName );
	}

	// 2009.08.15 nasukoji	�E�B���h�E���ɃA�N�Z�����[�^�e�[�u�����쐬����(Wine�p)
	::CheckDlgButton( hwndDlg, IDC_CHECK_ACCELTBL_EACHWIN, m_Common.m_bCreateAccelTblEachWin );

	return;
}



/* �_�C�A���O�f�[�^�̎擾 Keybind */
int CPropCommon::GetData_p5( HWND hwndDlg )
{
//@@@ 2002.01.03 YAZAKI �Ō�ɕ\�����Ă����V�[�g�𐳂����o���Ă��Ȃ��o�O�C��
//	m_nPageNum = ID_PAGENUM_KEYBOARD;
	// 2009.08.15 nasukoji	�E�B���h�E���ɃA�N�Z�����[�^�e�[�u�����쐬����(Wine�p)
	m_Common.m_bCreateAccelTblEachWin = ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_ACCELTBL_EACHWIN );

	return TRUE;
}

/*! Keybind: �L�[���X�g���`�F�b�N�{�b�N�X�̏�Ԃɍ��킹�čX�V���� */
void CPropCommon::p5_ChangeKeyList( HWND hwndDlg){
	HWND	hwndKeyList;
	int 	nIndex;
	int 	nIndexTop;
	int 	i;
	TCHAR	szKeyState[64];
	
	hwndKeyList = ::GetDlgItem( hwndDlg, IDC_LIST_KEY );
	nIndex = ::SendMessage( hwndKeyList, LB_GETCURSEL, 0, 0 );
	nIndexTop = ::SendMessage( hwndKeyList, LB_GETTOPINDEX, 0, 0 );
	_tcscpy( szKeyState, _T("") );
	i = 0;
	if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_SHIFT ) ){
		i |= _SHIFT;
		_tcscat( szKeyState, _T("Shift+") );
	}
	if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_CTRL ) ){
		i |= _CTRL;
		_tcscat( szKeyState, _T("Ctrl+") );
	}
	if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_ALT ) ){
		i |= _ALT;
		_tcscat( szKeyState, _T("Alt+") );
	}
	/* �L�[�ꗗ�ɕ�������Z�b�g�i���X�g�{�b�N�X�j*/
	::SendMessage( hwndKeyList, LB_RESETCONTENT, 0, 0 );
	for( i = 0; i < m_Common.m_nKeyNameArrNum; ++i ){
		TCHAR	pszLabel[256];
		wsprintf( pszLabel, _T("%s%s"), szKeyState, m_Common.m_pKeyNameArr[i].m_szKeyName );
		::SendMessage( hwndKeyList, LB_ADDSTRING, 0, (LPARAM)pszLabel );
	}
	::SendMessage( hwndKeyList, LB_SETCURSEL, nIndex, 0 );
	::SendMessage( hwndKeyList, LB_SETTOPINDEX, nIndexTop, 0 );
	::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_LIST_KEY, LBN_SELCHANGE ), (LPARAM)hwndKeyList );
}

/* Keybind:�L�[���蓖�Đݒ���C���|�[�g���� */
void CPropCommon::p5_Import_KeySetting( HWND hwndDlg )
{
	CDlgOpenFile	cDlgOpenFile;
	char			szPath[_MAX_PATH + 1];
	HFILE			hFile;

	char			pHeader[STR_KEYDATA_HEAD_LEN + 1];
	short			nKeyNameArrNum;				/* �L�[���蓖�ĕ\�̗L���f�[�^�� */
	const int		KEYNAME_SIZE = _countof(m_Common.m_pKeyNameArr);
	KEYDATA			pKeyNameArr[KEYNAME_SIZE];				/* �L�[���蓖�ĕ\ */
	HWND			hwndCtrl;
	char			szInitDir[_MAX_PATH + 1];

	strcpy( szPath, "" );
	strcpy( szInitDir, m_pShareData->m_szIMPORTFOLDER );	/* �C���|�[�g�p�t�H���_ */
	/* �t�@�C���I�[�v���_�C�A���O�̏����� */
	cDlgOpenFile.Create(
		m_hInstance,
		hwndDlg,
		_T("*.key"),
		szInitDir
	);
	if( !cDlgOpenFile.DoModal_GetOpenFileName( szPath ) ){
		return;
	}
	/* �t�@�C���̃t���p�X���A�t�H���_�ƃt�@�C�����ɕ��� */
	/* [c:\work\test\aaa.txt] �� [c:\work\test] + [aaa.txt] */
	::SplitPath_FolderAndFile( szPath, m_pShareData->m_szIMPORTFOLDER, NULL );
	strcat( m_pShareData->m_szIMPORTFOLDER, "\\" );

	hFile = _lopen( szPath, OF_READ );
	if( HFILE_ERROR == hFile ){
		::MYMESSAGEBOX(	hwndDlg, MB_OK | MB_ICONSTOP, GSTR_APPNAME,
			_T("�t�@�C�����J���܂���ł����B\n\n%s"), szPath
		);
		return;
	}
	if( STR_KEYDATA_HEAD_LEN		!= _lread( hFile, pHeader, STR_KEYDATA_HEAD_LEN ) ||
		sizeof( nKeyNameArrNum )	!= _lread( hFile, &nKeyNameArrNum, sizeof( nKeyNameArrNum ) ) ||
		sizeof( pKeyNameArr )		!= _lread( hFile,  pKeyNameArr,    sizeof( pKeyNameArr ) ) ||
		0 != memcmp( pHeader, STR_KEYDATA_HEAD, STR_KEYDATA_HEAD_LEN )
	){
		_lclose( hFile );  //@@@ 2001.11.07 add MIK

//@@@ 2001.11.07 add start MIK
		{
			FILE	*fp;
			int	i, j, cnt, kc, n, an;
			char	buff[1024], name[1024], szFuncNameJapanese[256], s[1024], *p, *q;

			if( (fp = fopen( szPath, "r" )) == NULL )
			{
				::MYMESSAGEBOX(	hwndDlg, MB_OK | MB_ICONSTOP, GSTR_APPNAME,
					_T("�t�@�C�����J���܂���ł����B\n\n%s"), szPath
				);
				return;
			}

			if( fgets(buff, sizeof(buff), fp) != NULL
			 && memcmp(buff, STR_KEYDATA_HEAD2, strlen(STR_KEYDATA_HEAD2)) == 0
			 && fgets(buff, sizeof(buff), fp) != NULL )
			{
				cnt = sscanf(buff, "Count=%d", &an);
				nKeyNameArrNum = an;
				if( cnt == 1 && an >= 0 && an <= KEYNAME_SIZE )
				{
					for(i = 0; i < an; i++)
					{
						name[0] = '\0';
						if( fgets(buff, sizeof(buff), fp) == NULL ) break;
						for(j = strlen(buff) - 1; j >= 0; j--){
							if( buff[j] == '\n' || buff[j] == '\r' ) buff[j] = '\0';
						}
						cnt = sscanf(buff, "KeyBind[%03d]=%04x,%s",
							&n,
							&kc,
							s,
							name);
						if( cnt != 3 ) break;
						if( i != n ) break;
						pKeyNameArr[i].m_nKeyCode = kc;

						p = s;
						for(j = 0; j < 8; j++)
						{
							q = strstr(p, ",");
							if( !q )
								break;
							*q = '\0';
							//�@�\���𐔒l�ɒu��������B(���l�̋@�\�������邩��)
							//@@@ 2002.2.2 YAZAKI �}�N����CSMacroMgr�ɓ���
//							n = CMacro::GetFuncInfoByName(m_hInstance, p, szFuncNameJapanese);
							n = CSMacroMgr::GetFuncInfoByName(m_hInstance, p, szFuncNameJapanese);
							if( n == -1 )
							{
								if( *p >= '0' && *p <= '9' )
								{
									n = atol(p);
								}
								else
								{
									n = 0;
								}
							}
							pKeyNameArr[i].m_nFuncCodeArr[j] = n;
							p = q + 1;
						}

						if( j != 8 ) break;

						strcpy(pKeyNameArr[i].m_szKeyName, p);
					}
					if( i == an )
					{
						fclose(fp);
						goto ToMaster;
					}
				}
			}
			fclose(fp);
		}
//@@@ 2001.11.07 add end MIK

		::MYMESSAGEBOX(	hwndDlg, MB_OK | MB_ICONSTOP, GSTR_APPNAME,
			_T("�L�[�ݒ�t�@�C���̌`�����Ⴂ�܂��B\n\n%s"), szPath
		);
		return;
	}
	_lclose( hFile );

ToMaster:	//@@@ 2001.11.07 add MIK
	/* �f�[�^�̃R�s�[ */
	m_Common.m_nKeyNameArrNum = nKeyNameArrNum;
	memcpy( m_Common.m_pKeyNameArr, pKeyNameArr, sizeof( pKeyNameArr ) );

//	CShareData::SetKeyNames( m_pShareData );	/* �L�[���̂̃Z�b�g */


//	/* �_�C�A���O�f�[�^�̐ݒ� p5 */
	// 2012.11.18 aroka �L�[�ꗗ�̍X�V�͑S�A�C�e�����X�V����B
	p5_ChangeKeyList( hwndDlg );
	//@@@ 2001.11.07 modify start MIK: �@�\�Ɋ��蓖�Ă��Ă���L�[���X�V����B// 2012.11.18 aroka �R�����g�C��
	hwndCtrl = ::GetDlgItem( hwndDlg, IDC_LIST_FUNC );
	::SendMessage( hwndDlg, WM_COMMAND, MAKELONG( IDC_LIST_FUNC, LBN_SELCHANGE ), (LPARAM)hwndCtrl );
	//@@@ 2001.11.07 modify end MIK
}


/* Keybind:�L�[���蓖�Đݒ���G�N�X�|�[�g���� */
void CPropCommon::p5_Export_KeySetting( HWND hwndDlg )
{
	CDlgOpenFile	cDlgOpenFile;
	char			szPath[_MAX_PATH + 1];
	char			szInitDir[_MAX_PATH + 1];
	const int KEYNAME_SIZE = _countof(m_Common.m_pKeyNameArr);

	strcpy( szPath, "" );
	strcpy( szInitDir, m_pShareData->m_szIMPORTFOLDER );	/* �C���|�[�g�p�t�H���_ */
	/* �t�@�C���I�[�v���_�C�A���O�̏����� */
	cDlgOpenFile.Create(
		m_hInstance,
		hwndDlg,
		_T("*.key"),
		szInitDir
	);
	if( !cDlgOpenFile.DoModal_GetSaveFileName( szPath ) ){
		return;
	}
	/* �t�@�C���̃t���p�X���A�t�H���_�ƃt�@�C�����ɕ��� */
	/* [c:\work\test\aaa.txt] �� [c:\work\test] + [aaa.txt] */
	::SplitPath_FolderAndFile( szPath, m_pShareData->m_szIMPORTFOLDER, NULL );
	strcat( m_pShareData->m_szIMPORTFOLDER, "\\" );

//@@@ 2001.11.07 add start MIK: �e�L�X�g�`���ŕۑ�
	{
		FILE	*fp;
		int	i, j;
		char	szFuncName[256], szFuncNameJapanese[256], *p;
		
		if( (fp = fopen( szPath, "w" )) == NULL )
		{
			::MYMESSAGEBOX(	hwndDlg, MB_OK | MB_ICONSTOP, GSTR_APPNAME,
				_T("�t�@�C�����J���܂���ł����B\n\n%s"), szPath
			);
			return;
		}
		
		fprintf(fp, "%s\n", STR_KEYDATA_HEAD2);
		fprintf(fp, "Count=%d\n", m_Common.m_nKeyNameArrNum);
		
		for(i = 0; i < KEYNAME_SIZE; i++)
		{
			fprintf(fp, "KeyBind[%03d]=%04x", i, m_Common.m_pKeyNameArr[i].m_nKeyCode);

			for(j = 0; j < 8; j++)
			{
				//@@@ 2002.2.2 YAZAKI �}�N����CSMacroMgr�ɓ���
//				p = CMacro::GetFuncInfoByID(m_hInstance, m_Common.m_pKeyNameArr[i].m_nFuncCodeArr[j], szFuncName, szFuncNameJapanese);
				p = CSMacroMgr::GetFuncInfoByID(m_hInstance, m_Common.m_pKeyNameArr[i].m_nFuncCodeArr[j], szFuncName, szFuncNameJapanese);
				if( p ) {
					fprintf(fp, ",%s", p);
				}
				else {
					fprintf(fp, ",%d", m_Common.m_pKeyNameArr[i].m_nFuncCodeArr[j]);
				}
			}
			
			fprintf(fp, ",%s\n", m_Common.m_pKeyNameArr[i].m_szKeyName);
		}
		
		fclose(fp);
	}
//@@@ 2001.11.07 add end MIK
}


/*[EOF]*/