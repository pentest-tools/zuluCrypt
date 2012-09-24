#include "mountpartition.h"
#include "ui_mountpartition.h"

mountPartition::mountPartition( QWidget * parent,QTableWidget * table ) :
	QWidget( parent ),m_ui(new Ui::mountPartition)
{
	m_ui->setupUi(this);
	m_table = table ;
	this->setFixedSize( this->size() );
	this->setWindowFlags( Qt::Window | Qt::Dialog );
	this->setFont( parent->font() );

	m_ui->checkBoxMountReadOnly->setCheckState( openvolumereadonly::getOption() );

	connect( m_ui->pbMount,SIGNAL( clicked() ),this,SLOT(pbMount() ) ) ;
	connect( m_ui->pbMountFolder,SIGNAL( clicked() ),this,SLOT( pbOpenMountPath() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->checkBox,SIGNAL( stateChanged( int ) ),this,SLOT( stateChanged( int ) ) ) ;
	connect( m_ui->checkBoxMountReadOnly,SIGNAL( stateChanged(int) ),this,SLOT( checkBoxReadOnlyStateChanged( int ) ) ) ;
	m_ui->pbMountFolder->setIcon( QIcon( QString( ":/folder.png" ) ) );

	userfont F( this ) ;
	this->setFont( F.getFont() );
}

void mountPartition::checkBoxReadOnlyStateChanged( int state )
{
	m_ui->checkBoxMountReadOnly->setEnabled( false );
	m_ui->checkBoxMountReadOnly->setChecked( openvolumereadonly::setOption( this,state) );
	m_ui->checkBoxMountReadOnly->setEnabled( true );
}

void mountPartition::pbCancel()
{
	this->HideUI();
}

void mountPartition::pbMount()
{
	managepartitionthread * part = new managepartitionthread() ;
	part->setDevice( m_path );
	if( m_ui->checkBoxMountReadOnly->isChecked() )
		part->setMode( QString( "ro" ) );
	else
		part->setMode( QString( "rw" ) );
	part->setMountPoint( m_ui->lineEdit->text() );
	connect( part,SIGNAL( signalMountComplete( int,QString ) ),this,SLOT( slotMountComplete( int,QString ) ) ) ;

	part->startAction( QString( "mount" ) ) ;
}

void mountPartition::pbOpenMountPath()
{
	QString p = tr( "Select Path to mount point folder" ) ;
	QString Z = QFileDialog::getExistingDirectory( this,p,QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( !Z.isEmpty() ){
		Z = Z + QString( "/" ) + m_ui->lineEdit->text().split( "/" ).last() ;
		m_ui->lineEdit->setText( Z );
	}
}

void mountPartition::ShowUI( QString path,QString label )
{
	m_path = path ;
	m_label = label ;

	QStringList opt = this->readOptions() ;

	QString m ;

	m = opt.at( 0 ) + path.split( "/" ).last() ;

	//if( opt.at( 1 ) == QString( "nolabel") )
	//	m = opt.at( 0 ) + path.split( "/" ).last() ;
	//else
	//	m = opt.at( 0 ) + label ;

	m_ui->lineEdit->setText( m );

	if( label == QString( "Nil" ) )
		m_ui->checkBox->setEnabled( false );
	this->show();
}

void mountPartition::stateChanged( int i )
{
	Q_UNUSED( i ) ;

	QString path = m_ui->lineEdit->text() ;

	int k = path.lastIndexOf( "/" ) ;

	if( k > 0 ){
		path = path.mid( 0,k + 1 ) ;

		if( m_ui->checkBox->isChecked() )
			m_ui->lineEdit->setText( path + m_label );
		else
			m_ui->lineEdit->setText( path + m_path.split( "/" ).last() );
	}
}

void mountPartition::volumeMiniProperties( QString prp )
{
	MainWindow::volumeMiniProperties( m_table,prp,m_ui->lineEdit->text() ) ;
	this->HideUI();
}

void mountPartition::slotMountComplete( int status,QString msg )
{
	if( status ){
		DialogMsg m( this ) ;
		m.ShowUIOK( QString( "ERROR" ),msg );
	}else{
		this->saveOptions( m_ui->lineEdit->text(),m_ui->checkBox->isChecked() );

		managepartitionthread * mpt = new managepartitionthread() ;
		mpt->setDevice( m_table->item( m_table->currentRow(),0 )->text() );
		connect( mpt,SIGNAL( signalProperties( QString ) ),this,SLOT( volumeMiniProperties( QString ) ) ) ;
		mpt->startAction( QString( "volumeMiniProperties" ) ) ;
	}
}
void mountPartition::HideUI()
{
	emit hideUISignal();
	this->hide();
}

void mountPartition::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->HideUI();
}

void mountPartition::saveOptions( QString path,bool mode )
{
	QString p = QDir::homePath() + QString( "/.zuluCrypt/zuluMount" ) ;
	QFile f( p ) ;

	f.open( QIODevice::WriteOnly ) ;

	path = path.mid( 0,path.lastIndexOf( "/") + 1 ) ;

	QString data ;

	if( mode )
		data = path + QString( "\nlabel\n" ) ;
	else
		data = path + QString( "\nnolabel\n" ) ;

	f.write( data.toAscii() ) ;

	f.close();
}

QStringList mountPartition::readOptions()
{
	QString path = QDir::homePath() + QString( "/.zuluCrypt/zuluMount" ) ;
	QFile f( path ) ;

	if( !f.exists() ){

		QString e = QDir::homePath() + QString( "/\nnolabel\n" ) ;
		f.open( QIODevice::WriteOnly ) ;
		f.write( e.toAscii() ) ;
		f.close();
	}

	f.open( QIODevice::ReadOnly ) ;

	QStringList data = QString( f.readAll() ).split( "\n" ) ;
	f.close();
	return data ;
}

mountPartition::~mountPartition()
{
	delete m_ui;
}

