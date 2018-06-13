#include <string>
#include <list>
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>

class CMyFileDialog
{
	const std::string c_title;
	const std::string c_ext;
	const std::string c_filter;
	std::string m_title;
	std::string m_ext;
	char *m_filter;
	std::list<std::string> m_files;
	HWND m_hOwnerWnd;
	DWORD m_ofnFlags;

	BOOL SafeGetOpenFileName(LPOPENFILENAMEA lpofn);
	BOOL SafeGetSaveFileName(LPOPENFILENAMEA lpofn);
	typedef BOOL (CMyFileDialog::*GetOpenSaveFileName)(LPOPENFILENAMEA);
	GetOpenSaveFileName getOpenSaveFileNamePtr;

	bool runDialog(LPOPENFILENAMEA lpofn, std::list<std::string>&);

	int strlen_doublenull(const char*);

public:
	CMyFileDialog();
	~CMyFileDialog();

	// "�J��" �_�C�A���O�̕W���ݒ��ǂݍ���
	void setOpenDlgDefaultSetting(void);

	// "���O��t���ĕۑ�" �_�C�A���O�̕W���ݒ��ǂݍ���
	void setSaveDlgDefaultSetting(void);

	// �_�C�A���O�̃^�C�g���������ݒ肷��
	void setTitle(const char*);

	// �_�C�A���O�̕W���g���q��ݒ肷��
	void setDefaultExt(const char*);

	// �_�C�A���O�̃t�B���^�[�������ݒ肷��
	void setFilter(const char*);

	//�_�C�A���O�̏��L�E�B���h�E��ݒ肷��
	void setOwner(HWND);

	// �_�C�A���O��W�J����
	bool Execute(void);

	// �o�b�t�@���̃t�@�C���p�X�������o��
	const char *getFile(int);

	// �o�b�t�@���̃��X�g���������o��
	int getIndex(void);
};

